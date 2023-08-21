#include "Commontypes.h"
#include <GLFW/glfw3.h>
std::vector<Cartesian3> vertices; //vector for vertices, contains x, y, z
std::vector<face> faces; //vector for the faces, contains vectors for each quad. in each vector, it has the index of the vertex
std::vector<face> newFaces; //vector for new faces
std::vector<Cartesian3> normals; //vector for normals
std::vector<Cartesian3> newVertices; //vector for new vertices


GLFWwindow* window = nullptr;