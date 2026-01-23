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
GLuint pl_program_uint;

GLuint model_to_camera_mat_unif;
GLuint camera_to_persp_mat_unif;
GLuint in_diffuse_color_unif;
GLuint camera_space_light_position_unif;
GLuint light_intensity_unif;
GLuint ambient_intensity_unif;
GLuint clip_to_camera_mat_unif;
GLuint window_size_unif;
GLuint light_attenuation_unif;

GLuint matrices_uniform_block_index;
GLuint matrices_UBO;
const int matrices_binding_index = 0;

glm::vec4 p_light_diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
GLuint p_light_diffuse_unif;

static float light_attenuation = 1.0f;

void initializeProgram(GLFWwindow* window){
    //Initialize shaders and programs here
    //Example shader loading:
    
    std::vector<GLuint> shaders;
    Shader vertex_shader("per_frag_light_fr.vert");
    Shader fragment_shader("per_frag_light_fr.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    model_to_camera_mat_unif = glGetUniformLocation(program_uint, "model_to_camera_mat");
    camera_to_persp_mat_unif = glGetUniformLocation(program_uint, "camera_to_persp_mat");
    in_diffuse_color_unif = glGetUniformLocation(program_uint, "in_diffuse_color");
    camera_space_light_position_unif = glGetUniformLocation(program_uint, "camera_space_light_position");
    light_intensity_unif = glGetUniformLocation(program_uint, "light_intensity");
    ambient_intensity_unif = glGetUniformLocation(program_uint, "ambient_intensity");
    clip_to_camera_mat_unif = glGetUniformLocation(program_uint, "clip_to_camera_mat");
    window_size_unif = glGetUniformLocation(program_uint, "window_size");
    light_attenuation_unif = glGetUniformLocation(program_uint, "light_attenuation");

    glUseProgram(program_uint);
    glUniform1f(light_attenuation_unif, light_attenuation);
    glUseProgram(0);

    //point light object shader, simple diffuse per vertex light
    std::vector<GLuint> pl_shaders;
    Shader pl_vertex_shader("plane_light.vert");
    Shader pl_frag_shader("per_frag_light.frag");
    pl_shaders.push_back(pl_vertex_shader.getShaderUint());
    pl_shaders.push_back(pl_frag_shader.getShaderUint());

    Program pl_program(pl_shaders);
    pl_program_uint = pl_program.getProgramUint();

    matrices_uniform_block_index = glGetUniformBlockIndex(program_uint, "Matrices");
    glUniformBlockBinding(program_uint, matrices_uniform_block_index, matrices_binding_index);
    
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 2);

    p_light_diffuse_unif = glGetUniformLocation(pl_program_uint, "diffuse_color");

    glUseProgram(pl_program_uint);
    glUniform4fv(p_light_diffuse_unif, 1, glm::value_ptr(p_light_diffuse));
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
    gltf_util::Model cube = loader.loadModel("box01.glb");

    //Check if normals exist
    if(!cube.normals.empty()) {
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
    //cube vao
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

int width;
int height;

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controler = nullptr;

void initializeCameras(GLFWwindow* window){
    glfwGetFramebufferSize(window, &width, &height);
    cam = std::make_unique<Camera>(width,height);
    cam_controler = std::make_unique<CameraController>(*cam);
    cam->position = glm::vec3(0.0f, 0.5f, 3.5f);
    cam_controler->movement_speed = 3.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void init(GLFWwindow* window){
    initializeProgram(window);
    initializeBuffers();
    initializeVertexArrayObjects();
    initializeCameras(window);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

bool draw_p_light = true;
bool rotate_p_light = true;
float p_light_movement_speed = 1.0f;
float y_pos = 0.5f;
float radius = 1.5f;

void processPointLightInput(GLFWwindow* window, float delta_time, float &y_pos, float &radius){
    float movement_distance = p_light_movement_speed * delta_time;

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        y_pos += movement_distance;

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        y_pos -= movement_distance;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        radius += movement_distance;

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        radius -= movement_distance;
}

float delta_time = 0.0f;
float last_frame = 0.0f;
float last_angle = 0.0f;
float angle = 0.0f;
float p_light_x;
float p_light_z;

glm::mat4 cube_model_mat(1);
glm::mat4 plane_model_mat = glm::translate(glm::mat4(1), glm::vec3(0.0f,-0.5f,0.0f)) * glm::scale(glm::mat4(1), glm::vec3(50,1,50));
glm::mat4 point_light_scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
glm::mat4 point_light_model_mat;

glm::vec4 cube_diffuse = glm::vec4(0.2, 0.2, 1.0, 1.0); //blue
glm::vec4 plane_diffuse = glm::vec4(0.5, 0.5, 0.5, 1.0); //silver

glm::vec4 light_intensity(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_intensity( 0.1f, 0.1f, 0.1f, 1.0f);

// Called every frame
void display(GLFWwindow* window){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //camera movement
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    cam_controler->processCameraInput(window, delta_time);
    cam->updateCamera();

    //===========
    //RENDER CUBE 
    //===========
    glUseProgram(program_uint);

    glm::mat4 cube_model_to_cam_mat = cam->getViewMat() * cube_model_mat;
    
    glUniformMatrix4fv(model_to_camera_mat_unif, 1, false, glm::value_ptr(cube_model_to_cam_mat));
    glUniformMatrix4fv(camera_to_persp_mat_unif, 1, false, glm::value_ptr(cam->getPerspMat()));
    glUniform4fv(light_intensity_unif, 1, glm::value_ptr(light_intensity));
    glUniform4fv(ambient_intensity_unif, 1, glm::value_ptr(ambient_intensity));
    glUniform4fv(in_diffuse_color_unif, 1, glm::value_ptr(cube_diffuse));

    glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);

    //==================
    //RENDER POINT LIGHT
    //==================
    glUseProgram(pl_program_uint);

    static float paused_time = 0.0f;
    float loop_duration = 10.0f;
    float curr_duration = fmod(current_frame - paused_time, loop_duration);

    if(rotate_p_light){
        angle = (curr_duration / loop_duration) * 2.0f * glm::pi<float>();
        p_light_x = radius * cos(angle);
        p_light_z = radius * sin(angle);
        last_angle = angle;
    }
    else{
        paused_time += delta_time;
        p_light_x = radius * cos(last_angle);
        p_light_z = radius * sin(last_angle);
    }

    processPointLightInput(window, delta_time, y_pos, radius);
 
    glm::vec3 p_light_translation_vec = glm::vec3(p_light_x, y_pos, p_light_z);

    point_light_model_mat = glm::translate(glm::mat4(1), p_light_translation_vec) * point_light_scale_mat;

    glm::mat4 mvp_mat_p_light = cam->getPerspMat() * cam->getViewMat() * point_light_model_mat;
    glm::mat4 mv_mat_p_light = cam->getViewMat() * point_light_model_mat; //isnt really used atm

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mvp_mat_p_light));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mv_mat_p_light));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //lil quirk
    glm::vec4 p_light_camera_pos = cam->getViewMat() * glm::vec4(p_light_translation_vec, 1.0f);

    glUseProgram(program_uint);
    glUniform3fv(camera_space_light_position_unif, 1, glm::value_ptr(glm::vec3(p_light_camera_pos)));
    
    glUseProgram(pl_program_uint);

    if(draw_p_light){
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, 0);
    }

	glBindVertexArray(0);

    //============
    //RENDER PLANE 
    //============
    glUseProgram(program_uint);

    glm::mat4 plane_model_to_cam_mat = cam->getViewMat() * plane_model_mat;
    
    glUniformMatrix4fv(model_to_camera_mat_unif, 1, false, glm::value_ptr(plane_model_to_cam_mat));
    glUniform4fv(in_diffuse_color_unif, 1, glm::value_ptr(plane_diffuse));

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

    glUseProgram(program_uint);
    glUniformMatrix4fv(clip_to_camera_mat_unif, 1, false, glm::value_ptr(glm::inverse(cam->getPerspMat())));
    glUniform2i(window_size_unif, width, height);
    glUseProgram(0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        draw_p_light = !draw_p_light;
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        rotate_p_light = !rotate_p_light;
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    cam_controler->mouseCameraController(window, x_pos, y_pos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    cam_controler->mouseZoomController(window, x_offset, y_offset);
}



