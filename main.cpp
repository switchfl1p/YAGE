#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Shader.hpp>
#include <Program.hpp>
#include <vector>

void init(){
    std::vector<GLuint> shaders;
    Shader vertex_shader("FragPosition.vert");
    Shader fragment_shader("FragPosition.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program theProgram(shaders);
}

const float vertexPositions[] = {
	0.2f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.1f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.3f, 1.0f,
};

glm::mat4 test(1.0f);

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
        std::cout << "Failed to initalize GLAD\n";
        glfwTerminate();
        return -1;
    }

    init();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
