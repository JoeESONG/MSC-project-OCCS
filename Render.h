#ifndef RENDER_H
#define RENDER_H

#include "Commontypes.h"
#include "Subdivision.h"
#include "FileRead.h"
#include "GLFW/glfw3.h"

extern GLfloat aspectRatio;
extern GLfloat windowWidth;
extern GLfloat windowHeight;


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void RenderScene();
void InitWindow();
void RunRenderLoop();
void Cleanup();
#endif // RENDER_H