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
#include <Light.hpp>
#include <Material.hpp>
#include <LightController.hpp>
#include <Node.hpp>
#include <core_util.hpp>

core_util::LitProgramData lit_program;
core_util::UnlitProgramData unlit_program;

void initializeProgram(){
    lit_program = core_util::loadLitProgram("shinies_0.vert", "shinies_1.frag");
    unlit_program = core_util::loadUnlitProgram("shinies_2.vert", "shinies_3.frag");
}

GLuint matrices_uniform_block_index;
GLuint matrices_UBO;
const int matrices_binding_index = 0;

void initializeUBOs(){
    //Create UBO and bind to binding index
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 2);

    //bind program uniform block index to binding index
    matrices_uniform_block_index = glGetUniformBlockIndex(lit_program.program_uint, "Matrices");
    glUniformBlockBinding(lit_program.program_uint, matrices_uniform_block_index, matrices_binding_index);

    matrices_uniform_block_index = glGetUniformBlockIndex(unlit_program.program_uint, "Matrices");
    glUniformBlockBinding(unlit_program.program_uint, matrices_uniform_block_index, matrices_binding_index);
}

Node bulb;
Node sphere;
Node plane;

void initializeNodes(){
    //Bulb
    Material bulb_material;
    bulb_material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //white
    bulb.material = bulb_material;
    Transform bulb_transform;
    bulb_transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
    bulb.transform = bulb_transform;

    glUseProgram(unlit_program.program_uint);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(bulb.material.diffuse_color));
    glUseProgram(0);

    //Centerpiece
    Material sphere_material;
    sphere_material.diffuse_color = glm::vec4(0.2, 0.2, 1.0, 1.0); //blue
    sphere.material = sphere_material;
    Transform sphere_transform;
    sphere_transform.scale_component = glm::vec3(0.5f, 0.5f, 0.5f);
    sphere_transform.calc_model_mat();
    sphere.transform = sphere_transform;
    
    //Plane
    Material plane_material;
    plane_material.diffuse_color = glm::vec4(0.5, 0.5, 0.5, 1.0); //silver
    plane.material = plane_material;
    Transform plane_transform;
    plane_transform.translation_component = glm::vec3(0.0f,-0.5f,0.0f);
    plane_transform.scale_component = glm::vec3(50,1,50);
    plane_transform.calc_model_mat();
    plane.transform = plane_transform;
}

AmbientLight ambient_light;
PointLight point_light;
LightController bulb_controller(point_light);

void initializeLights(){
    ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    point_light.attenuation = 1.0f;
    point_light.position = glm::vec3(0.0f, 0.25f, 0.0f);

    bulb_controller.radius = 1.0f;

    //send uniforms to program
    glUseProgram(lit_program.program_uint);
    glUniform4fv(lit_program.ambient_intensity_unif, 1, glm::value_ptr(ambient_light.intensity));
    glUniform4fv(lit_program.light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
    glUniform1f(lit_program.light_attenuation_unif, point_light.attenuation);
    glUseProgram(0);
}

GLuint vertex_buffer_object;
GLuint index_buffer_object;
GLuint normal_buffer_object;

int index_count = 0;

GLuint plane_vbo;
GLuint plane_ibo;
GLuint plane_nbo;

int plane_index_count = 0;

void initializeBuffers(){
    //Example usage:

    gltf_util::Loader loader;
    gltf_util::Model sphere = loader.loadModel("sphere.glb");

    //Check if normals exist
    if(!sphere.normals.empty()) {
        //NBO
        glGenBuffers(1, &normal_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sphere.vertex_count * 3 * sizeof(float), sphere.normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //VBO
    glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sphere.vertex_count * 3 * sizeof(float), sphere.positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //IBO
    glGenBuffers(1, &index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.index_count * sizeof(unsigned short), sphere.indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    index_count = sphere.index_count;

    gltf_util::Model plane = loader.loadModel("plane.glb");

    if(!plane.normals.empty()) {
        //NBO
        glGenBuffers(1, &plane_nbo);
        glBindBuffer(GL_ARRAY_BUFFER, plane_nbo);
        glBufferData(GL_ARRAY_BUFFER, plane.vertex_count * 3 * sizeof(float), plane.normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //VBO
    glGenBuffers(1, &plane_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, plane.vertex_count * 3 * sizeof(float), plane.positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //IBO
    glGenBuffers(1, &plane_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane.index_count * sizeof(unsigned short), plane.indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    plane_index_count = plane.index_count;
}

GLuint vao;
GLuint plane_vao;

void initializeVertexArrayObjects(){
    //sphere vao
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

    //plane vao
    glGenVertexArrays(1, &plane_vao);
    glBindVertexArray(plane_vao);

    //positions at attribute location 0
    glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (void*)0);

    //normals at attribute location 1
    glBindBuffer(GL_ARRAY_BUFFER, plane_nbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ibo);
    glBindVertexArray(0);
}

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controler = nullptr;

void initializeCameras(GLFWwindow* window){
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    cam = std::make_unique<Camera>(width,height);
    cam_controler = std::make_unique<CameraController>(*cam);
    cam->position = glm::vec3(0.0f, 0.0f, 2.5f);
    cam_controler->movement_speed = 3.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void init(GLFWwindow* window){
    initializeProgram();
    initializeUBOs();
    initializeNodes();
    initializeLights();
    initializeBuffers();
    initializeVertexArrayObjects();
    initializeCameras(window);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

float delta_time = 0.0f;
float last_frame = 0.0f;

// Called every frame
void display(GLFWwindow* window){
    //set bg color and clear depth buffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //delta time calcs
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    //camera movement
    cam_controler->processCameraInput(window, delta_time);
    cam->updateCamera();

    //bulb movement
    bulb_controller.rotatePointLight(current_frame, delta_time);
    bulb_controller.processPointLightInput(window, delta_time);

    //=============
    //RENDER SPHERE 
    //=============
    glUseProgram(lit_program.program_uint);

    glm::mat4 sphere_mv_mat = cam->getViewMat() * sphere.transform.model_mat;
    glm::vec4 p_light_camera_pos = cam->getViewMat() * glm::vec4(point_light.position, 1.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(sphere_mv_mat));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));

    glUniform4fv(lit_program.material_diffuse_unif, 1, glm::value_ptr(sphere.material.diffuse_color));
    glUniform3fv(lit_program.camera_space_light_position_unif, 1, glm::value_ptr(glm::vec3(p_light_camera_pos)));

    glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);

    //===========
    //RENDER BULB
    //===========
    glUseProgram(unlit_program.program_uint);

    bulb.transform.translation_component = point_light.position;
    bulb.transform.calc_model_mat();

    glm::mat4 bulb_mv_mat = cam->getViewMat() * bulb.transform.model_mat;

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb_mv_mat));
    
    if(bulb_controller.draw_flag){
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);
    }

	glBindVertexArray(0);

    //============
    //RENDER PLANE 
    //============
    glUseProgram(lit_program.program_uint);

    glm::mat4 plane_mv_mat = cam->getViewMat() * plane.transform.model_mat;
    
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(plane_mv_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUniform4fv(lit_program.material_diffuse_unif, 1, glm::value_ptr(plane.material.diffuse_color));

    glBindVertexArray(plane_vao);
    glDrawElements(GL_TRIANGLES, plane_index_count, GL_UNSIGNED_SHORT, 0);

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

    glUseProgram(lit_program.program_uint);
    glUniformMatrix4fv(lit_program.clip_to_camera_mat_unif, 1, false, glm::value_ptr(glm::inverse(cam->getPerspMat())));
    glUniform2i(lit_program.window_size_unif, width, height);
    glUseProgram(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    
    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        bulb_controller.rotate_flag = !bulb_controller.rotate_flag;
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        bulb_controller.draw_flag = !bulb_controller.draw_flag;

        if(bulb_controller.draw_flag){
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else{
            point_light.intensity = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        glUseProgram(lit_program.program_uint);
        glUniform4fv(lit_program.light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
        glUseProgram(0);
    }
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    cam_controler->mouseCameraController(window, x_pos, y_pos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    cam_controler->mouseZoomController(window, x_offset, y_offset);
}

void cleanup(){
    glDeleteProgram(lit_program.program_uint);
    glDeleteProgram(unlit_program.program_uint);

    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &plane_vao);

    glDeleteBuffers(1, &matrices_UBO);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteBuffers(1, &index_buffer_object);
    glDeleteBuffers(1, &normal_buffer_object);
    glDeleteBuffers(1, &plane_vbo);
    glDeleteBuffers(1, &plane_ibo);
    glDeleteBuffers(1, &plane_nbo);
}


