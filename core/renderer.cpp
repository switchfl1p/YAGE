/* switchfl1p 2025 */

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

void init(GLFWwindow* window);
void display(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void cleanup();

int main(){
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "YAGE", NULL, NULL);
    if (!window){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (gladLoadGL(glfwGetProcAddress) == 0){
        std::cerr << "Failed to initalize GLAD\n";
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 
    
    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window);
    }

    cleanup();
    
    glfwTerminate();
    return 0;
}