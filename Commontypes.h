#pragma once
#ifndef COMMONTYPES_H
#define COMMONTYPES_H
#include"GLFW/glfw3.h"
#include "MyVector.h"

extern std::vector<Cartesian3> vertices;
extern std::vector<face> faces;
extern std::vector<face> newFaces;
extern std::vector<Cartesian3> normals;
extern std::vector<Cartesian3> newVertices;


extern GLFWwindow* window;
#endif // COMMONTYPES_H