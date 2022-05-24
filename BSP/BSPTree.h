#include <iostream>
#include <vector>
#include <algorithm>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Face.h"
using namespace std;
using namespace glm;

const float eps1 = 0.001f;
const float eps2 = 0.00001f;
const float eps3 = 0.0f;

vec3 transformPoint(const mat4x4 &transformation, vec3 v);
vec3 transformVec(const mat4x4 &transformation, vec3 v);
int trianglePlaneIntersection(Face plane, Face triangle, vector<vec3> *outSegTips, vector<vec3> *outNormals);
void getSegmentPlaneIntersection(vec3 N, float D, vec3 p1, vec3 p2, vector<vec3> *outSegTips, vec3 n1, vec3 n2, vector<vec3> *outNormals);
float distFromPlane(vec3 N, float D, vec3 p);
bool insertIfNotIn(vector<vec3> *v, vec3 x);
vec3 getNormal(vec3 v1, vec3 v2, vec3 v3);

class BSPTree;
class Node;

class BSPTree
{
    public:
        void insertFaces(vector<Face> object, mat4x4 transformation, GLfloat *diffuse, GLfloat *specular, GLfloat *shininess, GLfloat *emission);
        void build();
        void classify(Face root, Face target, vector<Face> *frontFaces, vector<Face> *backFaces);
        void draw(const mat4x4 &transformMat);
    
    private:
        vector<Face> faces;
        vector<Face> frontFaces;
        vector<Face> backFaces;
        Node *root;

        Node *makeNode(const vector<Face> &facesToClassify);
        void drawNode(Node *n, const mat4x4 &transformMat);
};

struct Node
{
    Face face;
    Node *back; // Left child
    Node *front; // Right child
};