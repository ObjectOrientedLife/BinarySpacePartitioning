#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "objImporter.h"
using namespace std;

vector<Face> parseData(string path)
{
    ifstream file(path);
    if (file.is_open())
    {
        string line;
        vector<vector<string>> reservedFaceLines;
        vector<vec3> vertices;
        vector<vec3> vertexNormals;
        while (getline(file, line))
        {
            vector<string> sep = split(line, ' ');
            if (sep.size() == 0) // Comment line
            {
                continue;
            }

            string first = sep[0];
            if (first == "v")
            {
                vec3 vertex;
                vertex.x = stof(sep[1]);
                vertex.y = stof(sep[2]);
                vertex.z = stof(sep[3]);
                vertices.push_back(vertex);
            }
            else if (first == "vn")
            {
                vec3 normal;
                normal.x = stof(sep[1]);
                normal.y = stof(sep[2]);
                normal.z = stof(sep[3]);
                vertexNormals.push_back(normal);
            }
            else if (first == "f")
            {
                reservedFaceLines.push_back(sep);
            }
        }

        vector<Face> faces;
        for (vector<string> sep : reservedFaceLines)
        {
            string sep1 = sep[1]; // First vertex information
            string sep2 = sep[2];
            string sep3 = sep[3];

            vector<string> info1 = split(sep1, '/');
            vector<string> info2 = split(sep2, '/');
            vector<string> info3 = split(sep3, '/');

            int vertexIdx1 = stoi(info1[0]) - 1;
            vec3 vertex1 = vertices[vertexIdx1];
            int normalIdx1 = stoi(info1[2]) - 1;
            vec3 normal1 = vertexNormals[normalIdx1];

            int vertexIdx2 = stoi(info2[0]) - 1;
            vec3 vertex2 = vertices[vertexIdx2];
            int normalIdx2 = stoi(info2[2]) - 1;
            vec3 normal2 = vertexNormals[normalIdx2];

            int vertexIdx3 = stoi(info3[0]) - 1;
            vec3 vertex3 = vertices[vertexIdx3];
            int normalIdx3 = stoi(info3[2]) - 1;
            vec3 normal3 = vertexNormals[normalIdx3];

            // Define a face
            Face f;
            f.v1 = vertex1;
            f.v2 = vertex2;
            f.v3 = vertex3;

            f.n1 = normal1;
            f.n2 = normal2;
            f.n3 = normal3;

            faces.push_back(f);
        }

        return faces;
    }
    else
    {
        cout << path << " does not exist" << endl;
        return {};
    }
}

vector<string> split(string input, char delimiter) 
{
    vector<string> result;
    stringstream ss(input);
    string temp;
 
    while (getline(ss, temp, delimiter)) 
    {
        if (*temp.cbegin() == '#') // Ignore comments after the first #
        { 
            break;
        }
        result.push_back(temp);
    }
 
    return result;
}