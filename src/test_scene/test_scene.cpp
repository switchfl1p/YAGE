// test_scene.cpp
#include "glm/ext/matrix_clip_space.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <Shader.hpp>
#include <Program.hpp>

const float vertex_data[] = {
	 0.25f,  0.25f, -1.25f, 1.0f,
	 0.25f, -0.25f, -1.25f, 1.0f,
	-0.25f,  0.25f, -1.25f, 1.0f,

	 0.25f, -0.25f, -1.25f, 1.0f,
	-0.25f, -0.25f, -1.25f, 1.0f,
	-0.25f,  0.25f, -1.25f, 1.0f,

	 0.25f,  0.25f, -2.75f, 1.0f,
	-0.25f,  0.25f, -2.75f, 1.0f,
	 0.25f, -0.25f, -2.75f, 1.0f,

	 0.25f, -0.25f, -2.75f, 1.0f,
	-0.25f,  0.25f, -2.75f, 1.0f,
	-0.25f, -0.25f, -2.75f, 1.0f,

	-0.25f,  0.25f, -1.25f, 1.0f,
	-0.25f, -0.25f, -1.25f, 1.0f,
	-0.25f, -0.25f, -2.75f, 1.0f,

	-0.25f,  0.25f, -1.25f, 1.0f,
	-0.25f, -0.25f, -2.75f, 1.0f,
	-0.25f,  0.25f, -2.75f, 1.0f,

	 0.25f,  0.25f, -1.25f, 1.0f,
	 0.25f, -0.25f, -2.75f, 1.0f,
	 0.25f, -0.25f, -1.25f, 1.0f,

	 0.25f,  0.25f, -1.25f, 1.0f,
	 0.25f,  0.25f, -2.75f, 1.0f,
	 0.25f, -0.25f, -2.75f, 1.0f,

	 0.25f,  0.25f, -2.75f, 1.0f,
	 0.25f,  0.25f, -1.25f, 1.0f,
	-0.25f,  0.25f, -1.25f, 1.0f,

	 0.25f,  0.25f, -2.75f, 1.0f,
	-0.25f,  0.25f, -1.25f, 1.0f,
	-0.25f,  0.25f, -2.75f, 1.0f,

	 0.25f, -0.25f, -2.75f, 1.0f,
	-0.25f, -0.25f, -1.25f, 1.0f,
	 0.25f, -0.25f, -1.25f, 1.0f,

	 0.25f, -0.25f, -2.75f, 1.0f,
	-0.25f, -0.25f, -2.75f, 1.0f,
	-0.25f, -0.25f, -1.25f, 1.0f,




	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

};

GLuint program_uint;

GLuint model_mat_unif;
GLuint camera_mat_unif;
GLuint projection_mat_unif;

glm::mat4 perspective_mat;

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

    model_mat_unif = glGetUniformLocation(program_uint, "model_matrix");
    glm::mat4 model_mat(1);

    camera_mat_unif = glGetUniformLocation(program_uint, "camera_matrix");
    glm::vec3 camera_pos = glm::vec3(-1.0f, 1.5f, 3.0f);
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 camera_mat = glm::lookAt(camera_pos, camera_target, camera_up);

    projection_mat_unif = glGetUniformLocation(program_uint, "perspective_matrix");
    perspective_mat = glm::perspective(glm::radians(45.0f), (640.0f/480.0f), 0.1f, 100.0f);  

    glUseProgram(program_uint);
    glUniformMatrix4fv(model_mat_unif, 1, GL_FALSE, glm::value_ptr(model_mat));
    glUniformMatrix4fv(camera_mat_unif, 1, GL_FALSE, glm::value_ptr(camera_mat));
	glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(perspective_mat));
	glUseProgram(0);
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
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

void display(GLFWwindow* window) {
    // Rendering code here
    // Called every frame
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_uint);

    size_t color_data = sizeof(vertex_data) / 2;
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)color_data);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);

	glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    perspective_mat = glm::perspective(glm::radians(45.0f), float(width)/float(height), 0.1f, 100.0f);
    glUseProgram(program_uint);
    glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(perspective_mat));
    glUseProgram(0);

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
