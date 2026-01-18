#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <tiny_gltf.h>

#include <Shader.hpp>
#include <Program.hpp>
#include <Camera.hpp>
#include <CameraController.hpp>
#include <gltf_util.hpp>

GLuint program_uint;

GLuint matrices_uniform_block_index;
GLuint matrices_UBO;
const int matrices_binding_index = 0;

GLuint light_dir_unif;
GLuint light_intens_unif;
GLuint ambient_unif;

glm::mat4 model_mat(1);

int index_count = 0;

glm::vec4 light_direction(0.866f, 0.5f, 0.0f, 0.0f);
glm::vec4 light_intensity(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_intensity( 0.1f, 0.1f, 0.1f, 1.0f);

void initalizeProgram(GLFWwindow* window){
    //Initialize shaders and programs here
    //Example shader loading:
    
    std::vector<GLuint> shaders;
    Shader vertex_shader("gltf_util_usage.vert");
    Shader fragment_shader("gltf_util_usage.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    //UBO
    matrices_uniform_block_index = glGetUniformBlockIndex(program_uint, "Matrices");
    glUniformBlockBinding(program_uint, matrices_uniform_block_index, matrices_binding_index);
    
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 2);
    
    //light uniforms
    light_dir_unif = glGetUniformLocation(program_uint, "light_dir");
    light_intens_unif = glGetUniformLocation(program_uint, "light_intensity");
    ambient_unif = glGetUniformLocation(program_uint, "ambient_intensity");
}

GLuint vertex_buffer_object;
GLuint index_buffer_object;
GLuint normal_buffer_object;
GLuint vao;

void initalizeBuffers(){
    //Example usage:

    gltf_util::Loader loader;
    gltf_util::Model cube = loader.loadModel("box01.glb");

    //Check if normals exist
    if (!cube.normals.empty()) {
        //NBO
        glGenBuffers(1, &normal_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, cube.vertex_count * 3 * sizeof(float), cube.normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //VBO
    glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, cube.vertex_count * 3 * sizeof(float), cube.positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //IBO
    glGenBuffers(1, &index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.index_count * sizeof(unsigned short), cube.indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    index_count = cube.index_count;
}

void initializeVertexArrayObjects(){
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    //positions at attribute location 0
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);

    //normals at attribute location 1
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBindVertexArray(0);
}

int width;
int height;

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controler = nullptr;

void initalizeCameras(GLFWwindow* window){
    glfwGetFramebufferSize(window, &width, &height);
    cam = std::make_unique<Camera>(width,height);
    cam_controler = std::make_unique<CameraController>(*cam);
    cam->position = glm::vec3(0.0f, 1.0f, 5.0f);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}
//
void init(GLFWwindow* window){
    initalizeProgram(window);
    initalizeBuffers();
    initializeVertexArrayObjects();
    initalizeCameras(window);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

float delta_time = 0.0f;
float last_frame = 0.0f;

// Called every frame
void display(GLFWwindow* window){
    //set background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //camera movement
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    cam_controler->processCameraInput(window, delta_time);
    cam->updateCamera();
    glm::mat4 mvp_mat = cam->getPerspMat() * cam->getViewMat() * model_mat;

    //normals mat
    //assumes no non-uniform scaling, if so needs inverse transpose
    glm::mat4 mv_mat = cam->getViewMat() * model_mat;

    //light
    glm::vec4 light_direction_cam = cam->getViewMat() * light_direction;

    //use shaders
    glUseProgram(program_uint);
    
    //send to uniform block
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mvp_mat));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mv_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUniform4fv(light_dir_unif, 1, glm::value_ptr(light_direction_cam));
    glUniform4fv(light_intens_unif, 1, glm::value_ptr(light_intensity));
    glUniform4fv(ambient_unif, 1, glm::value_ptr(ambient_intensity));

    //render triangles
    glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    cam->viewport_w = width;
    cam->viewport_h = height;
    cam->updatePerspMat();

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    cam_controler->mouseCameraController(window, x_pos, y_pos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    cam_controler->mouseZoomController(window, x_offset, y_offset);
}



