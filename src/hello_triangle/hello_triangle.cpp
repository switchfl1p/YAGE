/* switchfl1p 2025 */

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Shader.hpp>
#include <Program.hpp>
#include <vector>

GLuint program_uint;
GLuint position_buffer_object;
GLuint vao;
GLint window_size_uniform;

const float vertex_positions[] = {
	0.2f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.1f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.3f, 1.0f,
};

void initalizeProgram(){
    std::vector<GLuint> shaders;
    Shader vertex_shader("frag_position.vert");
    Shader fragment_shader("frag_position.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    window_size_uniform = glGetUniformLocation(program_uint, "windowSize");
}

void initalizeVertexBuffer(){
    glGenBuffers(1, &position_buffer_object);

	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init(){
    initalizeProgram();
    initalizeVertexBuffer();

    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void display(GLFWwindow* window){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_uint);
    glUniform1f(window_size_uniform, 500.0f);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

