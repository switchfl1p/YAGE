#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <iomanip>

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    int platform = glfwGetPlatform();
    std::cout << platform << "\n";

    glfwMakeContextCurrent(window);

    // LOAD GLAD
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        std::cout << "Failed to initalize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // ----- GLM test -----
    glm::vec3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(4.0f, 5.0f, 6.0f);
    glm::vec3 c = a + b;
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "GLM vector sum: (" << c.x << ", " << c.y << ", " << c.z << ")\n";

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
