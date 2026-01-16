#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
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

GLuint mvp_mat_unif;
GLuint norm_view_mat_unif;

GLuint light_dir_unif;
GLuint light_intens_unif;
GLuint ambient_unif;

int index_count = 0;

glm::mat4 model_mat(1);

glm::vec4 light_direction(0.866f, 0.5f, 0.0f, 0.0f);
glm::vec4 light_intensity(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_intensity( 0.1f, 0.1f, 0.1f, 1.0f);

void initalizeProgram(GLFWwindow* window){
    //Initialize shaders and programs here
    //Example shader loading:
    
    std::vector<GLuint> shaders;
    Shader vertex_shader("test.vert");
    Shader fragment_shader("test.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    //transform uniform
    mvp_mat_unif = glGetUniformLocation(program_uint, "mvp_mat");

    //normals transform uniform
    norm_view_mat_unif = glGetUniformLocation(program_uint, "norm_view_mat");

    //light uniforms
    light_dir_unif = glGetUniformLocation(program_uint, "light_dir");
    light_intens_unif = glGetUniformLocation(program_uint, "light_intensity");
    ambient_unif = glGetUniformLocation(program_uint, "ambient_intensity");
}

GLuint vertex_buffer_object;
GLuint index_buffer_object;
GLuint normal_buffer_object;
GLuint vao;

void initalizeVertexBuffer(){
    //Example usage:

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

    //Get first mesh (the cube)
    const tinygltf::Mesh &mesh = model.meshes[0];
    const tinygltf::Primitive &primitive = mesh.primitives[0];

    //Get vertex positions
    const tinygltf::Accessor &pos_accessor = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView &posView = model.bufferViews[pos_accessor.bufferView];
    const tinygltf::Buffer &pos_buffer = model.buffers[posView.buffer];

    const float *positions = reinterpret_cast<const float*>(
        &pos_buffer.data[posView.byteOffset + pos_accessor.byteOffset]
    );

    int vertex_count = pos_accessor.count;

    //Check if normals exist
    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        const tinygltf::Accessor &norm_accessor = model.accessors[primitive.attributes.at("NORMAL")];
        const tinygltf::BufferView &norm_view = model.bufferViews[norm_accessor.bufferView];
        const tinygltf::Buffer &norm_buffer = model.buffers[norm_view.buffer];

        const float *normals = reinterpret_cast<const float*>(
            &norm_buffer.data[norm_view.byteOffset + norm_accessor.byteOffset]
        );
        
        //NBO
        glGenBuffers(1, &normal_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(float), normals, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //get indices
    const tinygltf::Accessor &ind_accessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &ind_view = model.bufferViews[ind_accessor.bufferView];
    const tinygltf::Buffer &ind_buffer = model.buffers[ind_view.buffer];
    
    const unsigned short *indices = reinterpret_cast<const unsigned short*>(
        &ind_buffer.data[ind_view.byteOffset + ind_accessor.byteOffset]
    );

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

    //positions at attribute location 0
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);

    //normals at attribute location 1
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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

    //normals mat3
    glm::mat4 mv_mat = cam->getViewMat() * model_mat;
    glm::mat3 norm_mat = glm::mat3(mv_mat);

    //uncomment in case of non uniform scaling
    //glm::mat3 norm_mat = glm::mat3(glm::transpose(glm::inverse(mv_mat)));

    //light
    glm::vec4 light_direction_cam = cam->getViewMat() * light_direction;

    //use shaders
    glUseProgram(program_uint);
    
    //send uniforms
    glUniformMatrix4fv(mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp_mat));
    glUniformMatrix3fv(norm_view_mat_unif, 1, GL_FALSE, glm::value_ptr(norm_mat));

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


