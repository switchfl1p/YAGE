#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <memory>
#include <tiny_gltf.h>

#include <Shader.hpp>
#include <Program.hpp>
#include <Camera.hpp>
#include <CameraController.hpp>

tinygltf::Model model;
tinygltf::TinyGLTF loader;
std::string err;
std::string warn;
std::string filename = "meshes/box01.glb";

GLuint program_uint;

GLuint model_mat_unif;
GLuint camera_mat_unif;
GLuint projection_mat_unif;

int index_count = 0;

void initalizeProgram(GLFWwindow* window){
    // Initialize shaders and programs here
    // Example shader loading:

    std::vector<GLuint> shaders;
    Shader vertex_shader("object_loading.vert");
    Shader fragment_shader("object_loading.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    model_mat_unif = glGetUniformLocation(program_uint, "model_matrix");
    camera_mat_unif = glGetUniformLocation(program_uint, "camera_matrix");
    projection_mat_unif = glGetUniformLocation(program_uint, "perspective_matrix");

    glm::mat4 model_mat(1);

    glUseProgram(program_uint);
    glUniformMatrix4fv(model_mat_unif, 1, GL_FALSE, glm::value_ptr(model_mat));
	glUseProgram(0);
}

GLuint vertex_buffer_object;
GLuint index_buffer_object;
GLuint vao;

void initalizeVertexBuffer(){
    // Example usage:

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF: %s\n", filename.c_str());
    }

    // Get first mesh (the cube)
    const tinygltf::Mesh &mesh = model.meshes[0];
    const tinygltf::Primitive &primitive = mesh.primitives[0];

    // Get vertex positions
    const tinygltf::Accessor &pos_accessor = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView &posView = model.bufferViews[pos_accessor.bufferView];
    const tinygltf::Buffer &pos_buffer = model.buffers[posView.buffer];

    const float *positions = reinterpret_cast<const float*>(
        &pos_buffer.data[posView.byteOffset + pos_accessor.byteOffset]
    );

    const tinygltf::Accessor &ind_accessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &ind_view = model.bufferViews[ind_accessor.bufferView];
    const tinygltf::Buffer &ind_buffer = model.buffers[ind_view.buffer];
    
    const unsigned short *indices = reinterpret_cast<const unsigned short*>(
        &ind_buffer.data[ind_view.byteOffset + ind_accessor.byteOffset]
    );

    int vertex_count = pos_accessor.count; // 32
    index_count = ind_accessor.count;

    //VBO
    glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //IBO
    glGenBuffers(1, &index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeVertexArrayObjects(){
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void init(GLFWwindow* window){
    initalizeProgram(window);
    initalizeVertexBuffer();
    initializeVertexArrayObjects();
    initalizeCameras(window);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

float delta_time = 0.0f;
float last_frame = 0.0f;

void display(GLFWwindow* window){
    // Rendering code here
    // Called every frame

    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam_controler->processCameraInput(window, delta_time);
    cam->updateCamera();

    glUseProgram(program_uint);
    
    glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(cam->getPerspMat()));
    glUniformMatrix4fv(camera_mat_unif, 1, GL_FALSE, glm::value_ptr(cam->getViewMat()));

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

    glUseProgram(program_uint);
    glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(cam->getPerspMat()));
    glUseProgram(0);

    glViewport(0, 0, width, height);
    glfwGetFramebufferSize(window, &width, &height);
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

