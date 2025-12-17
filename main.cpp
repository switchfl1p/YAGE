/* switchfl1p 2025 */

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Shader.hpp>
#include <Program.hpp>
#include <vector>

GLuint program_uint;
GLuint position_buffer_object;
GLuint vao;

const float vertex_positions[] = {
	0.2f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.1f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.3f, 1.0f,
};

void initalizeProgram(){
    std::vector<GLuint> shaders;
    Shader vertex_shader("FragPosition.vert");
    Shader fragment_shader("FragPosition.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program theProgram(shaders);
    program_uint = theProgram.getProgramUint();
}

void initalizeVertexBuffer(){
    glGenBuffers(1, &position_buffer_object);

	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init(){
    initalizeProgram();
}

void reshape(int w, int h){
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void display();
void keyboard();

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

    //program not deleted
    glUseProgram(program_uint);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
