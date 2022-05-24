#ifndef FACE
#define FACE

#include <GL/gl.h>
#include <glm/glm.hpp>
using namespace glm;

struct Face
{
    // Vertices
    vec3 v1; 
    vec3 v2;
    vec3 v3;

    // Normals
    vec3 n1; 
    vec3 n2;
    vec3 n3;
};

#endif