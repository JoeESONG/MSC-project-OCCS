#include <io.h>
#include <iostream>
#include "MyVector.h"
#include <windows.h>
#include <shobjidl.h> 

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include <unordered_map>
#include"FileRead.h"
#include"Subdivision.h"
#include"Render.h"
#include"GLFW/glfw3.h"
#include"Commontypes.h"
float Fa = 0.0f; 
int main()
{
  
    InitWindow();
    
    initVertices();
    
    RunRenderLoop();

    Cleanup();
   
    glfwTerminate();

    return 0;
}