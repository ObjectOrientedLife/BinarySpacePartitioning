#ifndef FACE
#define FACE

#include <GL/gl.h>
#include <glm/glm.hpp>
using namespace glm;

struct Face
{
    Face() {}

    Face(vec3 _v1, vec3 _v2, vec3 _v3, vec3 _n1, vec3 _n2, vec3 _n3, GLfloat *_diffuse, GLfloat *_specular, GLfloat *_shininess, GLfloat *_emission)
    : v1(_v1), v2(_v2), v3(_v3), 
    n1(_n1), n2(_n2), n3(_n3),
    diffuse(_diffuse), specular(_specular), shininess(_shininess), emission(_emission) 
    {}
    
    // Vertices
    vec3 v1; 
    vec3 v2;
    vec3 v3;

    // Normals
    vec3 n1; 
    vec3 n2;
    vec3 n3;

    // Elements
    GLfloat *diffuse = nullptr;
    GLfloat *specular = nullptr;
    GLfloat *shininess = nullptr;
    GLfloat *emission = nullptr;
};

#endif