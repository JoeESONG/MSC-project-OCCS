#include "Render.h"
#include "Commontypes.h"
#include "Subdivision.h"
#include "FileRead.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_impl_opengl2.h>
 GLfloat aspectRatio;
 GLfloat windowWidth;
 GLfloat windowHeight;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

float mouseX, mouseY;
float angleX, angleY;
float scale = 6;
bool mouseLeftButtonDown;
GLfloat scaleFactor = 1.0f;

// Callback function for mouse button events
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mouseLeftButtonDown = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mouseLeftButtonDown = false;
        }
    }
}

// Callback function for mouse movement
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

    // If mouse is interacting with ImGui, don't process the mouse event
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (mouseLeftButtonDown)
    {
        // If initial mouse position not recorded, set the positions
        if (mouseX == 0 && mouseY == 0) {
            mouseX = xpos;
            mouseY = ypos;
        }

        // Calculate rotation angles based on mouse movement
        angleY += (xpos - mouseX);
        angleX += (ypos - mouseY);
        mouseX = xpos;
        mouseY = ypos;
    }
    else
    {
        mouseX = xpos;
        mouseY = ypos;
    }
}

// Callback function for window resize
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Update viewport with new width and height
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Calculate the aspect ratio
    aspectRatio = (GLfloat)width / (GLfloat)height;
    windowWidth = 150.0 * aspectRatio;
    windowHeight = windowWidth;
    // Set the orthographic projection
    glOrtho(-windowWidth, windowWidth, -windowHeight, windowHeight, 1500, -1500);

}
// 初始化窗口和OpenGL上下文
void InitWindow()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        // Handle initialization failure
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Catmull-Clark Demo", NULL, NULL);
    if (!window)
    {
        // Handle window creation failure
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set the current window as the rendering context
    glfwMakeContextCurrent(window);

    // Set window resize callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Set mouse button callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // Set mouse movement callback
    glfwSetCursorPosCallback(window, cursorPosCallback);
    // Uncomment below for keyboard callback
    //glfwSetKeyCallback(window, keyCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vertical sync (vsync)

    // ImGui: setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Set ImGui style
    ImGui::StyleColorsDark();

    // Initialize ImGui for OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Set OpenGL context parameters
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    aspectRatio = (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT;
    windowWidth = 150.0 * aspectRatio;
    windowHeight = windowWidth;
    glOrtho(-windowWidth, windowWidth, -windowHeight, windowHeight, 1500, -1500);
    glMatrixMode(GL_MODELVIEW);
}
// Render the scene
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    glScalef(scale * 6, scale * 6, scale * 6);

    glColor3f(0.0f, 1.0f, 1.0f);

    for (int i = 0; i < faces.size(); i++)
    {
        faces[i].lines.clear();

        int j = 0;
        // Draw 3 lines
        for (j = 0; j < faces[i].points.size() - 1; j++)
        {
            glBegin(GL_LINES);

            Cartesian3 startVertice = vertices[faces[i].points[j] - 1];
            Cartesian3 endVertice = vertices[faces[i].points[j + 1] - 1];
            glVertex3f(startVertice.x, startVertice.y, startVertice.z);
            glVertex3f(endVertice.x, endVertice.y, endVertice.z);

            edge* Line = new edge(faces[i].points[j], faces[i].points[j + 1]);
            faces[i].lines.push_back(Line);
            vertices[Line->start - 1].lines.push_back(Line);
            vertices[Line->end - 1].lines.push_back(Line);

            glEnd();
        }

        glBegin(GL_LINES);

        Cartesian3 startVertice = vertices[faces[i].points[j] - 1];
        Cartesian3 endVertice = vertices[faces[i].points[0] - 1];
        glVertex3f(startVertice.x, startVertice.y, startVertice.z);
        glVertex3f(endVertice.x, endVertice.y, endVertice.z);

        edge* Line = new edge(faces[i].points[j], faces[i].points[0]);
        faces[i].lines.push_back(Line);
        vertices[Line->start - 1].lines.push_back(Line);
        vertices[Line->end - 1].lines.push_back(Line);

        glEnd();
    }

    glPopMatrix();

}

void RunRenderLoop()
{
    while (!glfwWindowShouldClose(window))
    {
     
      
        glfwPollEvents();

        // ImGui: start new frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Set next window size and collapsed state
        ImGui::SetNextWindowSize(ImVec2(500, 200)); // Set a larger size
       

        // ImGui: add controls
        ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize); // Disallow user resizing
        extern float Fa;

        ImGui::SliderFloat("Scale", &scale, 0.0f, 10.0f);
        ImGui::SliderFloat("Threshold", &Fa, 0.0f, 10.0f); 
        static int sliderValue = 0;
        if (ImGui::SliderInt("Subdivision", &sliderValue, 0, 4)) {
            Adaptive_Subdivision();
        }
        // Add a button to open file dialog
        if (ImGui::Button("Chose Your Model")) {
            initVertices();
        }
        if (ImGui::Button("Clear Scene")) {
            vertices.clear();
            faces.clear();
            Fa = 0.0; // Reset threshold
            scale = 6.0; // Reset scale
            sliderValue = 0;
 }
        

        if (ImGui::Button("Output")) {
            outputToOBJ("output.obj", vertices, faces);

        }
      
        ImGui::End();
      
       
        RenderScene();
        // ImGui: render
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
      
    }
}
void Cleanup()
{
   
    // ImGui: cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}