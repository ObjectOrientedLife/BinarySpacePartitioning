#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "objImporter.h"
#include "BSPTree.h"
using namespace std;
using namespace glm;

// ==================== Function declarations ====================
int main(int argc, char** argv);
void init();
void display();
void reshape(int w, int h);
void drawScene(GLenum mode);

void setLight0();
void setLight1();
void setLight2();

void insertLED();
void insertThinkPad();
void insertPanel();
void insertPlane();
void insertBackground();
void insertGoldenSphere();
void insertSilverSphere();
void insertSapphireSphere();
void insertTrackPoint();
vector<Face> getSphere(float radius, int segment);
vector<Face> getQuad(float x, float y);

void mouseClick(int button, int state, int x, int y);
GLboolean select(GLint x, GLint y);
vector<GLdouble> pick(GLint x, GLint y);
void mouseMovement(int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void showAll();
vector<GLfloat> normalize2(vector<GLfloat> v);
vector<GLfloat> cross(vector<GLfloat> a, vector<GLfloat> b);
GLfloat magnitude(vector<GLfloat> v);
void printMat(mat4x4 m);
void copyMat(GLfloat *src, GLfloat *dst);
void printVec(vec3 v);
void drawObj(const vector<Face> &mesh);
mat4x4 getCurrentTranform();

// ==================== Global variables ====================
static GLfloat ratio = 0.0;
static GLfloat windowW = 1000.0;
static GLfloat windowH = 1000.0;
static GLfloat nearClip = 1.0;
static GLfloat farClip = 500.0;

static const GLint BUFFER_SIZE = 512;

enum Movement
{
    ROTATION,
    SHIFT,
    DOLLY,
    ZOOM
};
static Movement movement = ROTATION;

static GLfloat startMousePosX = 0.0;
static GLfloat startMousePosY = 0.0;

static GLfloat camX = 0.0;
static GLfloat camY = 0.0;
static GLfloat camZ = 5.0;
static GLfloat lookAtX = 0.0;
static GLfloat lookAtY = 0.0;
static GLfloat lookAtZ = 0.0;

GLfloat *prevShiftRotationMat = new GLfloat[16];
GLfloat *currShiftRotationMat = new GLfloat[16];

static const GLfloat dollySensitivity = 0.1;
GLfloat *prevDollyMat = new GLfloat[16];
GLfloat *currDollyMat = new GLfloat[16];
static GLfloat initDollyAmount = -10.0;
static GLfloat dollyAmount = 0.0;

static const GLfloat rotationSensitivity = 0.5;
static GLfloat rotateAmount = 0.0;
static GLfloat axisX = 0.0;
static GLfloat axisY = 0.0;
static GLfloat axisZ = 0.0;

static const GLfloat shiftSensitivity = 0.05;
static GLfloat shiftAmountX = 0.0;
static GLfloat shiftAmountY = 0.0;

static GLboolean zoomInMode = false;
static const GLfloat zoomSensitivity = 0.2;
static GLfloat fov = 60.0;
static GLfloat fovOffset = 0.0;

static GLboolean selectMode = false;
static vector<GLdouble> rotationCenter = {0.0, 0.0, initDollyAmount};

static GLfloat showAllDolly = -350.0;

// ==================== Object variables ====================
vector<Face> led;
vector<Face> thinkPad;
vector<Face> panel;
vector<Face> plane;
vector<Face> background;
vector<Face> sphere;
vector<Face> key;
vector<Face> trackPoint;
vector<Face> cube;

BSPTree bt;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowW, windowH); 
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    init();

    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMovement);

    glutKeyboardFunc(keyboardDown);

    glutMainLoop();
    
    return 0;
}

void init() 
{
    glClearColor(1, 1, 1, 1);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    setLight0();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // ==================== Test2 ====================
    led = parseData("./Models/LED.obj");
    thinkPad = parseData("./Models/ThinkPad.obj");
    panel = parseData("./Models/Panel.obj");
	plane = parseData("./Models/Plane.obj");
	background = getQuad(10, 10);
    sphere = getSphere(0.5f, 8);
    key = parseData("./Models/Key.obj");
    trackPoint = parseData("./Models/TrackPoint.obj");
    cube = parseData("./Models/Cube.obj");

    // ==================== Build a BSP tree ====================
    glPushMatrix();
		insertPlane(); // insert a base plane
		glTranslatef(2, 0.2, 0);
    glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 5, -5);
		glRotatef(90, 1, 0, 0);
		insertBackground();
	glPopMatrix();

    glPushMatrix();
		glTranslatef(0, 1.8, 0);
		glRotatef(45, 0, 0, 1);

		glPushMatrix();
			glTranslatef(-0.6, 0.4, 0.2);
			insertLED(); // insert an LED
		glPopMatrix();

		glPushMatrix();
			glRotatef(90, 1, 0, 0);
			insertThinkPad(); // insert a ThinkPad logo
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 0, -0.5);
			glRotatef(90, 1, 0, 0);
			insertPanel(); // insert a glass panel begind the logo
		glPopMatrix();
    glPopMatrix();

    glPushMatrix();
		glTranslatef(1.2, 0.5, 1.5);
		insertGoldenSphere(); // insert a golden key 
    glPopMatrix();

    glPushMatrix();
		glTranslatef(2.4, 0.5, 0); // insert a silver key 
		insertSilverSphere();
    glPopMatrix();

    glPushMatrix();
		glTranslatef(2.4, 0.5, 1.5); // insert a sapphire key 
		insertSapphireSphere();
    glPopMatrix();

    glPushMatrix();
		glTranslatef(1.8, 0.7, 0.75); // insert a track point
		insertTrackPoint();
    glPopMatrix();

    bt.build();

    // ==================== Initialize the view ====================
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, prevShiftRotationMat);
    glTranslatef(0.0, 0.0, initDollyAmount);
    glGetFloatv(GL_MODELVIEW_MATRIX, prevDollyMat);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Zoom - separated since it affects picking
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov + fovOffset, ratio, nearClip, farClip);

    drawScene(GL_MODELVIEW);
    glutSwapBuffers();
}

void drawScene(GLenum mode)
{
    glMatrixMode(mode);

    // Set dolly matrix
    glLoadIdentity();
    glTranslatef(0.0, 0.0, dollyAmount);
    glMultMatrixf(prevDollyMat);
    glGetFloatv(GL_MODELVIEW_MATRIX, currDollyMat);

    // Combine rotation matrix and shift matrix into one
    GLdouble centerX = rotationCenter[0];
    GLdouble centerY = rotationCenter[1];
    GLdouble centerZ = rotationCenter[2] - prevDollyMat[14];
    glLoadIdentity();
    glTranslatef(centerX, centerY, centerZ); // Restore
    glRotatef(rotateAmount, axisX, axisY, axisZ);
    glTranslatef(-centerX, -centerY, -centerZ); // Center at rotationCenter
    glTranslatef(shiftAmountX, shiftAmountY, 0.0); // Translation
    glMultMatrixf(prevShiftRotationMat);
    glGetFloatv(GL_MODELVIEW_MATRIX, currShiftRotationMat);

    // Multiply matrices and draw
    glLoadIdentity();
    glMultMatrixf(currDollyMat);
    glMultMatrixf(currShiftRotationMat);

    // ==================== Draw by traversing the BSP tree ====================
    GLfloat *transformArr = new GLfloat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, transformArr);
    mat4x4 transformMat = make_mat4x4(transformArr);
    bt.draw(transformMat);
	
	// ==================== Set the lights ====================
	glPushMatrix();
		glTranslatef(1.8, 0.3, 0.4);
		setLight2();
    glPopMatrix();

    glPushMatrix();
		glTranslatef(0, 1.8, 0);
		glRotatef(45, 0, 0, 1);
		glPushMatrix();
			glTranslatef(-0.6, 0.4, 0.2);
			setLight1(); // And set its light emission
		glPopMatrix();
    glPopMatrix();
}

void mouseClick(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) // Button down
	{
		startMousePosX = x;
		startMousePosY = y;
		if (selectMode)
		{
			if (button == GLUT_LEFT_BUTTON)
			{
			GLboolean selected = select(x, y);
			if (selected)
			{
				rotationCenter = pick(x, y);
			}
			selectMode = false;
			}
			return;
		}
		switch (button)
		{
			case GLUT_LEFT_BUTTON:
			movement = ROTATION;
			break;
			case GLUT_MIDDLE_BUTTON:
			if (zoomInMode)
			{
				movement = ZOOM;
			}
			else
			{
				movement = DOLLY;
			}
			break;
			case GLUT_RIGHT_BUTTON:
			movement = SHIFT;
			break;
		}
	}
	else if (state == GLUT_UP) // Button up
	{
		switch (movement)
		{
			case ROTATION:
			copyMat(currShiftRotationMat, prevShiftRotationMat);
			rotateAmount = 0.0;
			axisX = 0.0;
			axisY = 0.0;
			axisZ = 0.0;
			break;
			case DOLLY:
			copyMat(currDollyMat, prevDollyMat);
			rotationCenter[2] += dollyAmount;
			dollyAmount = 0;
			break;
			case ZOOM:
			fov = fov + fovOffset;
			fovOffset = 0.0;
			break;
			case SHIFT:
			copyMat(currShiftRotationMat, prevShiftRotationMat);
			shiftAmountX = 0;
			shiftAmountY = 0;
			break;
		}
	}
}

GLboolean select(GLint x, GLint y) // Mouse position x, y
{
	GLuint hitBuffer[64];
	GLint hits, viewport[4];

	glSelectBuffer(64, hitBuffer);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
		glRenderMode(GL_SELECT);
		glLoadIdentity();
		gluPickMatrix(x, viewport[3] - y, 0.1, 0.1, viewport);
		gluPerspective(fov, ratio, nearClip, farClip);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		drawScene(GL_SELECT);

		hits = glRenderMode(GL_RENDER);
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();

    glutPostRedisplay();
    
	glMatrixMode(GL_MODELVIEW);

    return hits > 0;
}

vector<GLdouble> pick(GLint x, GLint y)
{
	vector<GLdouble> pos3D(3, 0);
	GLdouble projection[16];
	GLdouble modelView[16];
	GLint viewport[4];

	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);

	GLdouble winX = (double) x;
	GLdouble winY = (double) viewport[3] - (double) y;
	GLfloat winZ = 0.0;
	glReadPixels((int) winX, (int) winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	GLint result = gluUnProject(winX, winY, winZ, modelView, projection, viewport, &pos3D[0], &pos3D[1],&pos3D[2]);

	return pos3D;
}

void mouseMovement(int x, int y)
{
	if (selectMode)
	{
		return;
	}

	GLfloat xOffset = x - startMousePosX;
	GLfloat yOffset = startMousePosY - y;
	vector<GLfloat> newVec = {xOffset, yOffset};

	GLfloat m = magnitude(newVec);
	vector<GLfloat> unit = normalize2(newVec);
	unit.push_back(0.0); // Z = 0.0;

	vector<float> startVector = {0.0, 0.0, 1.0};
	vector<float> axis = cross(startVector, unit);

	switch (movement)
	{
		case ROTATION:
			rotateAmount = m * rotationSensitivity;
			axisX = axis[0];
			axisY = axis[1];
			axisZ = axis[2];
			break;
		case DOLLY:
			dollyAmount = yOffset * dollySensitivity;
			break;
		case ZOOM:
			fovOffset = -yOffset * zoomSensitivity;
			break;
		case SHIFT:
			shiftAmountX = xOffset * shiftSensitivity;
			shiftAmountY = yOffset * shiftSensitivity;
			break;
	}

	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y)
{
    switch (key)
    {
		case 'z':
			zoomInMode = !zoomInMode;
			break;
		case 's':
			selectMode = true;
			break;
		case 'a':
			showAll();
			break;
    }
}

void showAll()
{
	glLoadIdentity();
	glTranslatef(0.0, 0.0, showAllDolly);
	glGetFloatv(GL_MODELVIEW_MATRIX, prevDollyMat);

	prevShiftRotationMat[12] = 0.0;
	prevShiftRotationMat[13] = 0.0;

	rotationCenter[2] = showAllDolly;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ratio = (GLfloat) w / (GLfloat) h;
	windowW = w;
	windowH = h;
	gluPerspective(fov, ratio, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
}

vector<GLfloat> normalize2(vector<GLfloat> v)
{ 
	GLfloat magnitude = sqrt(v[0] * v[0] + v[1] * v[1]);
	return {v[0] / magnitude, v[1] / magnitude};
}

GLfloat magnitude(vector<GLfloat> v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1]);
}

vector<GLfloat> cross(vector<GLfloat> a, vector<GLfloat> b)
{
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[1]};
}

void copyMat(GLfloat *src, GLfloat *dst)
{
	for (int i = 0; i < 16; ++i)
	{
	  dst[i] = src[i];
    }
}

void printVec(vec3 v)
{
    cout << v.x << " " << v.y << " " << v.z << endl;
}

void printMat(mat4x4 m)
{
    cout << m[0].x << " " << m[0].y << " " << m[0].z << " " << m[0].w << endl;
    cout << m[1].x << " " << m[1].y << " " << m[1].z << " " << m[1].w << endl;
    cout << m[2].x << " " << m[2].y << " " << m[2].z << " " << m[2].w << endl;
    cout << m[3].x << " " << m[3].y << " " << m[3].z << " " << m[3].w << endl;
}

void drawObj(const vector<Face> &mesh)
{
    for (Face f : mesh)
    {
	  glBegin(GL_TRIANGLES);
		 glNormal3f(f.n1.x, f.n1.y, f.n1.z);
		 glVertex3f(f.v1.x, f.v1.y, f.v1.z);

		 glNormal3f(f.n2.x, f.n2.y, f.n2.z);
		 glVertex3f(f.v2.x, f.v2.y, f.v2.z);

		 glNormal3f(f.n3.x, f.n3.y, f.n3.z);
		 glVertex3f(f.v3.x, f.v3.y, f.v3.z);
	  glEnd();
    }
}

mat4x4 getCurrentTranform()
{
    GLfloat trans[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, trans);
    mat4x4 transMat = make_mat4x4(trans);
    return transMat;
}

// ==================== Functions that set the material property of each object and draw it ====================
void insertLED()
{
    GLfloat *diffuse = new GLfloat[4]{1, 0, 0, 0.8};
    GLfloat *specular = new GLfloat[4]{0.79,0.33,0.33, 0.8};
    GLfloat *shininess = new GLfloat[1]{100};
    GLfloat *emission = new GLfloat[4]{1, 0, 0, 1};

    bt.insertFaces(led, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertThinkPad()
{
	GLfloat *diffuse = new GLfloat[4]{1, 1, 1, 1}; // Diffuse color
	GLfloat *specular = new GLfloat[4]{1, 1, 1, 1}; // Amount of specular reflection of each component
	GLfloat *shininess = new GLfloat[1]{3}; // Specular range; higher value results a narrower specular reflection range
	GLfloat *emission = new GLfloat[4]{0, 0, 0, 1}; // Emiting color

	bt.insertFaces(thinkPad, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertPanel()
{
    GLfloat *diffuse = new GLfloat[4]{0.8, 1, 1, 0.25};
    GLfloat *specular = new GLfloat[4]{1, 1, 1, 1};
    GLfloat *shininess = new GLfloat[1]{100};
    GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

    bt.insertFaces(panel, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertPlane()
{
    GLfloat *diffuse = new GLfloat[4]{0.1, 0.1, 0.1, 1};
    GLfloat *specular = new GLfloat[4]{0.1, 0.1, 0.1, 1};
    GLfloat *shininess = new GLfloat[1]{3};
    GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

    bt.insertFaces(plane, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertBackground()
{
	GLfloat *diffuse = new GLfloat[4]{0.1, 0.1, 0.1, 1};
    GLfloat *specular = new GLfloat[4]{0.1, 0.1, 0.1, 1};
    GLfloat *shininess = new GLfloat[1]{3};
    GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

    bt.insertFaces(background, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertGoldenSphere()
{
	GLfloat *diffuse = new GLfloat[4]{0.88, 0.75, 0.3, 1};
	GLfloat *specular = new GLfloat[4]{1, 0.84, 0, 1};
	GLfloat *shininess = new GLfloat[1]{10};
	GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

	bt.insertFaces(sphere, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertSilverSphere()
{
	GLfloat *diffuse = new GLfloat[4]{0.7, 0.7, 0.7, 1};
	GLfloat *specular = new GLfloat[4]{1, 1, 1, 1};
	GLfloat *shininess = new GLfloat[1]{128};
	GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

	bt.insertFaces(sphere, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertSapphireSphere()
{
	GLfloat *diffuse = new GLfloat[4]{0.37, 0.45, 1, 0.5};
	GLfloat *specular = new GLfloat[4]{0.87, 0.86, 1, 1};
	GLfloat *shininess = new GLfloat[1]{128};
	GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};

	bt.insertFaces(sphere, getCurrentTranform(), diffuse, specular, shininess, emission);
}

void insertTrackPoint()
{
	GLfloat *diffuse = new GLfloat[4]{1, 0.09, 0.11, 1};
	GLfloat *specular = new GLfloat[4]{1, 0.59, 0.6, 1};
	GLfloat *shininess = new GLfloat[1]{5};
	GLfloat *emission = new GLfloat[4]{0, 0, 0, 1};
    
	bt.insertFaces(trackPoint, getCurrentTranform(), diffuse, specular, shininess, emission);
}

vector<Face> getSphere(float radius, int segment) // The center is located at (0, 0, 0)
{
	vector<Face> faces;
	for (float phi = 0.0f; phi < M_PI; phi += M_PI / segment)
	{
		for (float theta = 0.0f; theta < (2.0f + 1e-2f) * M_PI; theta += M_PI / segment)
		{
			float x;
			float y;
			float z;

			// Face 1
			Face f1;
			
			x = radius * cos(theta) * sin(phi);
			y = radius * sin(theta) * sin(phi);
			z = radius * cos(phi);
			f1.v1 = vec3(x, y, z);
			f1.n1 = normalize(vec3(x, y, z));

			x = radius * cos(theta + M_PI / segment) * sin(phi);
			y = radius * sin(theta + M_PI / segment) * sin(phi);
			z = radius * cos(phi);
			f1.v2 = vec3(x, y, z);
			f1.n2 = normalize(vec3(x, y, z));
			
			x = radius * cos(theta) * sin(phi + M_PI / segment);
			y = radius * sin(theta) * sin(phi + M_PI / segment);
			z = radius * cos(phi + M_PI / segment);
			f1.v3 = vec3(x, y, z);
			f1.n3 = normalize(vec3(x, y, z));

			faces.push_back(f1);

			// Face2
			Face f2;
			
			x = radius * cos(theta) * sin(phi + M_PI / segment);
			y = radius * sin(theta) * sin(phi + M_PI / segment);
			z = radius * cos(phi + M_PI / segment);
			f2.v1 = vec3(x, y, z);
			f2.n1 = normalize(vec3(x, y, z));

			x = radius * cos(theta + M_PI / segment) * sin(phi);
			y = radius * sin(theta + M_PI / segment) * sin(phi);
			z = radius * cos(phi);
			f2.v2 = vec3(x, y, z);
			f2.n2 = normalize(vec3(x, y, z));

			x = radius * cos(theta + M_PI / segment) * sin(phi + M_PI / segment);
			y = radius * sin(theta + M_PI / segment) * sin(phi + M_PI / segment);
			z = radius * cos(phi + M_PI / segment);
			f2.v3 = vec3(x, y, z);
			f2.n3 = normalize(vec3(x, y, z));

			faces.push_back(f2);
		}
	}

	return faces;
}

vector<Face> getQuad(float x, float z)
{
	float halfX = x / 2.0f;
	float halfZ = z / 2.0f;
	vector<Face> faces;

	// Make f1
	Face f1;
	f1.v1 = vec3(-halfX, 0, halfZ);
	f1.n1 = vec3(0, 1, 0);

	f1.v2 = vec3(halfX, 0, -halfZ);
	f1.n2 = vec3(0, 1, 0);

	f1.v3 = vec3(-halfX, 0, -halfZ);
	f1.n3 = vec3(0, 1, 0);

	faces.push_back(f1);

	// Make f2
	Face f2;
	f2.v1 = vec3(halfX, 0, halfZ);
	f2.n1 = vec3(0, 1, 0);

	f2.v2 = vec3(halfX, 0, -halfZ);
	f2.n2 = vec3(0, 1, 0);

	f2.v3 = vec3(-halfX, 0, halfZ);
	f2.n3 = vec3(0, 1, 0);

	faces.push_back(f2);

	return faces;
}

// ==================== Functions that set the light properties ====================
void setLight0()
{
	glEnable(GL_LIGHT0);
	GLfloat lightDiffuse[] = {1, 1, 1, 1};
	GLfloat lightPosition[] = {0, 10, 10, 0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void setLight1()
{
	glEnable(GL_LIGHT1);
	GLfloat lightDiffuse[] = {1, 0, 0, 1};
	GLfloat lightPosition[] = {0, 0, 0, 1}; // w == 1: Positional light
	GLfloat lightQuadraticAtten[] = {0.5};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, lightQuadraticAtten);
}

void setLight2()
{
	glEnable(GL_LIGHT2);
	GLfloat lightDiffuse[] = {56, 85, 100, 1};
	GLfloat lightPosition[] = {0, 0, 0, 1}; // w == 1: Positional light
	GLfloat lightDirection[] = {0, 1, 0};
	GLfloat lightQuadraticAtten[] = {30};
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDirection);
	glLightfv(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, lightQuadraticAtten);
}