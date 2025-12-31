// test_scene.cpp
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <Shader.hpp>
#include <Program.hpp>

void initalizeProgram(){
    // Initialize shaders and programs here
    // Example shader loading:

    std::vector<GLuint> shaders;
    Shader vertex_shader("test_scene.vert");
    Shader fragment_shader("test_scene.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();
}

GLuint vertex_buffer_object;
GLuint vao;

void initalizeVertexBuffer(){
    // Example usage:

    glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init() {
    initalizeProgram();
    initalizeVertexBuffer();

    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

void display(GLFWwindow* window) {
    // Rendering code here
    // Called every frame
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
