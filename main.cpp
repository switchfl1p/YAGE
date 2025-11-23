#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>           // Core GLM functionality
#include <glm/gtc/matrix_transform.hpp> // For transforms (optional)
#include <glm/gtc/type_ptr.hpp>        // For value_ptr (optional)
#include <cstdio>

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // LOAD GLAD
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }

    // ----- GLM test -----
    glm::vec3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(4.0f, 5.0f, 6.0f);
    glm::vec3 c = a + b; // Simple vector addition
    printf("GLM vector sum: (%f, %f, %f)\n", c.x, c.y, c.z);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
