#include "BSPTree.h"

void BSPTree::insertFaces(vector<Face> object, mat4x4 transformation, GLfloat *diffuse, GLfloat *specular, GLfloat *shininess, GLfloat *emission)
{
    mat4x4 normalTransformation = transformation; // Normals don't take effect of translation 
    normalTransformation[3].x = 0;
    normalTransformation[3].y = 0;
    normalTransformation[3].z = 0;

    for (Face face : object)
    {
        Face transformedFace;

        transformedFace.v1 = transformPoint(transformation, face.v1);
        transformedFace.v2 = transformPoint(transformation, face.v2);
        transformedFace.v3 = transformPoint(transformation, face.v3);

        transformedFace.n1 = transformPoint(normalTransformation, face.n1);
        transformedFace.n2 = transformPoint(normalTransformation, face.n2);
        transformedFace.n3 = transformPoint(normalTransformation, face.n3);

        transformedFace.diffuse = diffuse;
        transformedFace.specular = specular;
        transformedFace.shininess = shininess;
        transformedFace.emission = emission;

        faces.push_back(transformedFace);
    }
}

void BSPTree::build()
{
    root = makeNode(faces);
}

Node *BSPTree::makeNode(const vector<Face> &facesToClassify)
{
    if (facesToClassify.size() == 0) // Check first
    {
        return nullptr;
    }

    Node *node = new Node(); // Dynamically allocate to prevent from being deleted

    Face plane = *facesToClassify.begin(); // Plane
    node->face = plane;

    vector<Face> frontFaces;
    vector<Face> backFaces;

    for (int i = 1; i < facesToClassify.size(); ++i)
    {
        Face target = facesToClassify[i];
        classify(plane, target, &frontFaces, &backFaces);
    }

    node->front = makeNode(frontFaces);
    node->back = makeNode(backFaces);

    return node;
}

void BSPTree::classify(Face root, Face target, vector<Face> *frontFaces, vector<Face> *backFaces)
{
    vec3 N = getNormal(root.v1, root.v2, root.v3);
    float D = -(N.x * root.v1.x + N.y * root.v1.y + N.z * root.v1.z); // Define the plane equation
    vec3 v1 = target.v1;
    vec3 v2 = target.v2;
    vec3 v3 = target.v3;
    vec3 n1 = target.n1;
    vec3 n2 = target.n2;
    vec3 n3 = target.n3;
    GLfloat *diffuse = target.diffuse;
    GLfloat *specular = target.specular;
    GLfloat *shininess = target.shininess;
    GLfloat *emission = target.emission;
    
    vector<vec3> intersections;
    vector<vec3> normals;
    int flag = trianglePlaneIntersection(root, target, &intersections, &normals);
    
    vector<Face> unclassified;
    if (intersections.size() == 2)
    {
        vec3 i1 = intersections[0]; // First intersection point
        vec3 i2 = intersections[1]; // Second intersection point
        vec3 in1 = normals[0];
        vec3 in2 = normals[1];

        if (flag == 3)
        {
            Face f1(v1, i1, i2, n1, in1, in2, diffuse, specular, shininess, emission); // Fix normals!
            Face f2(i1, v2, i2, in1, n2, in2, diffuse, specular, shininess, emission);
            Face f3(v1, i2, v3, n1, in2, n3, diffuse, specular, shininess, emission);

            unclassified.push_back(f1);
            unclassified.push_back(f2);
            unclassified.push_back(f3);
        }
        else if (flag == 5)
        {
            Face f1(v1, i1, i2, n1, in1, in2, diffuse, specular, shininess, emission); // Fix normals!
            Face f2(i1, v2, i2, in1, n2, in2, diffuse, specular, shininess, emission);
            Face f3(i2, v2, v3, in2, n2, n3, diffuse, specular, shininess, emission);

            unclassified.push_back(f1);
            unclassified.push_back(f2);
            unclassified.push_back(f3);
        }
        else if (flag == 6)
        {
            Face f1(v1, v2, i1, n1, n2, in1, diffuse, specular, shininess, emission); // Fix normals!
            Face f2(v1, i1, i2, n1, in1, in2, diffuse, specular, shininess, emission);
            Face f3(i2, i1, v3, in2, in1, n3, diffuse, specular, shininess, emission);

            unclassified.push_back(f1);
            unclassified.push_back(f2);
            unclassified.push_back(f3);
        }
    }
    else
    {
        unclassified.push_back(target);
    }

    for (Face f : unclassified)
    {
        if (distance(f.v1, f.v2) > eps2 && distance(f.v2, f.v3) > eps2 && distance(f.v3, f.v1) > eps2)
        {
            if (distFromPlane(N, D, f.v1) + distFromPlane(N, D, f.v2) + distFromPlane(N, D, f.v3) >= eps2) // On the front side
            {
                frontFaces->push_back(f);
            }
            else // On the back side
            {
                backFaces->push_back(f);
            }
        }
    }
}

vec3 transformPoint(const mat4x4 &transformation, vec3 v) // Column major vs Row major...?
{
    vec4 homogeneous(v.x, v.y, v.z, 1);
    vec4 transformed = transformation * homogeneous;
    return vec3(transformed.x, transformed.y, transformed.z);
}

vec3 transformVec(const mat4x4 &transformation, vec3 v) // Column major vs Row major...?
{
    vec4 homogeneous(v.x, v.y, v.z, 0);
    vec4 transformed = transformation * homogeneous;
    return vec3(transformed.x, transformed.y, transformed.z);
}

int trianglePlaneIntersection(Face plane, Face triangle, vector<vec3> *outSegTips, vector<vec3> *outNormals)
{
    vec3 N = getNormal(plane.v1, plane.v2, plane.v3);
    float D = -(N.x * plane.v1.x + N.y * plane.v1.y + N.z * plane.v1.z);

    int flag = 0;
    int prevSize = 0;
    getSegmentPlaneIntersection(N, D, triangle.v1, triangle.v2, outSegTips, triangle.n1, triangle.n2, outNormals);
    flag |= (outSegTips->size() - prevSize > 0);
    prevSize = outSegTips->size();
    getSegmentPlaneIntersection(N, D, triangle.v2, triangle.v3, outSegTips, triangle.n2, triangle.n3, outNormals);
    flag |= (outSegTips->size() - prevSize > 0) << 1;
    prevSize = outSegTips->size();
    getSegmentPlaneIntersection(N, D, triangle.v3, triangle.v1, outSegTips, triangle.n3, triangle.n1, outNormals);
    flag |= (outSegTips->size() - prevSize > 0) << 2;

    return flag;
}

void getSegmentPlaneIntersection(vec3 N, float D, vec3 p1, vec3 p2, vector<vec3> *outSegTips, vec3 n1, vec3 n2, vector<vec3> *outNormals)
{
    float d1 = distFromPlane(N, D, p1);
    float d2 = distFromPlane(N, D, p2);

    bool isP1OnPlane = abs(d1) < eps1;
    bool isP2OnPlane = abs(d2) < eps1;

    if (isP1OnPlane || isP2OnPlane)
    {
        return;
    }
    else if (isP1OnPlane)
    {
        outSegTips->push_back(p1);
        outNormals->push_back(n1);
    }
    else if (isP2OnPlane)
    {
        outSegTips->push_back(p2);
        outNormals->push_back(n2);
    }

    if (d1 * d2 > eps3)  // Points on the same side of plane
    {
        return;
    }

    float t = d1 / (d1 - d2); // 'time' of intersection point on the segment
    vec3 intersection = p1 + t * (p2 - p1);
    vec3 normal = n1 + t * (n2 - n1);
    if (insertIfNotIn(outSegTips, intersection))
    {
        outNormals->push_back(normal);
    }
}

bool insertIfNotIn(vector<vec3> *v, vec3 x)
{
    bool hasElement = false;
    for (auto e : *v)
    {
        hasElement |= (distance(x, e) < eps1);
    }
    if (!hasElement)
    {
        v->push_back(x);
    }

    return !hasElement;
}

float distFromPlane(vec3 N, float D, vec3 p)
{
    return dot(N, p) + D;
}

vec3 getNormal(vec3 v1, vec3 v2, vec3 v3)
{
    return normalize(cross(v2 - v1, v3 - v1));
}

void BSPTree::draw(const mat4x4 &transformMat)
{
    drawNode(root, transformMat);
}

void BSPTree::drawNode(Node *n, const mat4x4 &transformMat)
{
    Face f = n->face;
    vec3 centroid = transformPoint(transformMat, (f.v1 + f.v2 + f.v3) / 3.0f);
    vec3 faceNormal = transformVec(transformMat, getNormal(f.v1, f.v2, f.v3));
    vec3 viewNormal = normalize(vec3(0, 0, 0) - centroid);
    bool isFacingFront = dot(faceNormal, viewNormal) >= 0.0f;

    if (isFacingFront)
    {
        if (n->back != nullptr)
        {
            drawNode(n->back, transformMat);
        }

        // Set the material
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, f.diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, f.specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, f.shininess);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, f.emission);

        glBegin(GL_TRIANGLES);
            glNormal3f(f.n1.x, f.n1.y, f.n1.z);
            glVertex3f(f.v1.x, f.v1.y, f.v1.z);

            glNormal3f(f.n2.x, f.n2.y, f.n2.z);
            glVertex3f(f.v2.x, f.v2.y, f.v2.z);

            glNormal3f(f.n3.x, f.n3.y, f.n3.z);
            glVertex3f(f.v3.x, f.v3.y, f.v3.z);
        glEnd();

        if (n->front != nullptr)
        {
            drawNode(n->front, transformMat);
        }
    }
    else
    {
        if (n->front != nullptr)
        {
            drawNode(n->front, transformMat);
        }

        // Set the material
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, f.diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, f.specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, f.shininess);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, f.emission);

        Face f = n->face;
        glBegin(GL_TRIANGLES);
            glNormal3f(f.n1.x, f.n1.y, f.n1.z);
            glVertex3f(f.v1.x, f.v1.y, f.v1.z);

            glNormal3f(f.n2.x, f.n2.y, f.n2.z);
            glVertex3f(f.v2.x, f.v2.y, f.v2.z);

            glNormal3f(f.n3.x, f.n3.y, f.n3.z);
            glVertex3f(f.v3.x, f.v3.y, f.v3.z);
        glEnd();

        if (n->back != nullptr)
        {
            drawNode(n->back, transformMat);
        }
    }

}