//=============== switchfl1p 2025-2026 ==================

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <tiny_gltf.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <iostream>

#include <Shader.hpp>
#include <Program.hpp>
#include <Camera.hpp>
#include <CameraController.hpp>
#include <gltf_util.hpp>
#include <Light.hpp>
#include <LightController.hpp>
#include <Node.hpp>
#include <core_util.hpp>
#include <renderGUI.hpp>
#include <FrameBuffer.hpp>

//========================================================

std::vector<core_util::LitProgramData*> lit_programs;
core_util::LitProgramData lambertian_program;
core_util::LitProgramData phong_program;
core_util::LitProgramData blinn_program;
core_util::LitProgramData gaussian_program;
core_util::LitProgramData pbr_program;

core_util::UnlitProgramData unlit_program;

//program switching helper
core_util::LitProgramData* current_program;
enum LightingModel
{
    LM_LAMBERTIAN = 0,
    LM_PHONG_LIGHTING,
    LM_BLINN_LIGHTING,
    LM_GAUSSIAN_LIGHTING,
    LM_PBR_LIGHTING,

    LM_COUNT,
};
int light_model = LM_PBR_LIGHTING;

GLuint matrices_uniform_block_index;
GLuint matrices_UBO;
const int matrices_binding_index = 0;

std::unordered_map<std::string, Node> nodes;

AmbientLight ambient_light;
PointLight point_light;
LightController bulb_controller(point_light);

core_util::ModelData sphere_data;
core_util::ModelData plane_data;

core_util::VAOData sphere_vao;
core_util::VAOData plane_vao;

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controler = nullptr;
bool camera_movement_flag;

float delta_time = 0.0f;
float last_frame = 0.0f;

std::unique_ptr<Framebuffer> viewport_fb = nullptr;

//========================================================

void initializePrograms(){
    lambertian_program = core_util::loadLitProgram("pass_normals.vert", "lambertian.frag");
    phong_program = core_util::loadLitProgram("pass_normals.vert", "phong.frag");
    blinn_program = core_util::loadLitProgram("pass_normals.vert", "blinn_phong.frag");
    gaussian_program = core_util::loadLitProgram("pass_normals.vert", "gaussian.frag");
    pbr_program = core_util::loadLitProgram("pass_normals.vert", "pbr.frag");

    //No Light
    unlit_program = core_util::loadUnlitProgram("simple.vert", "no_light.frag");

    lit_programs.push_back(&lambertian_program);
    lit_programs.push_back(&phong_program);
    lit_programs.push_back(&blinn_program);
    lit_programs.push_back(&gaussian_program);
    lit_programs.push_back(&pbr_program);
}

void initializeUBOs(){
    //Create UBO and bind to binding index
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 2);

    //handle lit programs
    for(auto* program : lit_programs){
        matrices_uniform_block_index = glGetUniformBlockIndex(program->program_uint, "Matrices");
        glUniformBlockBinding(program->program_uint, matrices_uniform_block_index, matrices_binding_index);
    }

    //handle unlit ones
    matrices_uniform_block_index = glGetUniformBlockIndex(unlit_program.program_uint, "Matrices");
    glUniformBlockBinding(unlit_program.program_uint, matrices_uniform_block_index, matrices_binding_index);
}

void initializeNodes(){
    //Bulb
    {
        Node bulb;
        Material bulb_material;
        bulb_material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //white
        bulb.material = bulb_material;

        Transform bulb_transform;
        bulb_transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
        bulb.transform = bulb_transform;

        nodes["bulb"] = bulb;

        glUseProgram(unlit_program.program_uint);
        glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(bulb.material.diffuse_color));
        glUseProgram(0);
    }

    //Centerpiece
    {
        Node sphere;
        Material sphere_material;
        sphere_material.diffuse_color = glm::vec4(0.2, 0.2, 1.0, 1.0); //blue
        sphere_material.shininess_factor = 64.0f;
        //pbr
        sphere_material.base_color = glm::vec4(0.0, 0.0, 1.5, 1.0);
        sphere_material.metallic = 0.0f;   // plastic/ceramic
        sphere_material.roughness = 0.30f;  // somewhat shiny
        sphere.material = sphere_material;

        Transform sphere_transform;
        sphere_transform.scale_component = glm::vec3(0.5f, 0.5f, 0.5f);
        sphere_transform.calc_model_mat();
        sphere.transform = sphere_transform;

        nodes["sphere"] = sphere;
    }

    //Plane
    {
        Node plane;
        Material plane_material;
        plane_material.diffuse_color = glm::vec4(0.5, 0.5, 0.5, 1.0); //silver
        plane_material.shininess_factor = 64.0f;

        //pbr
        plane_material.base_color = glm::vec4(0.8, 0.8, 0.8, 1.0);
        plane_material.metallic = 0.0f;
        plane_material.roughness = 0.7f;  // matte
        plane.material = plane_material;

        Transform plane_transform;
        plane_transform.translation_component = glm::vec3(0.0f,-0.5f,0.0f);
        plane_transform.calc_model_mat();
        plane.transform = plane_transform;

        nodes["plane"] = plane;
    }
}

void initializeLights(){
    ambient_light.intensity = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
    
    point_light.intensity = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    point_light.attenuation = 1.0f;
    point_light.position = glm::vec3(0.0f, 0.25f, 0.0f);

    bulb_controller.radius = 1.0f;

    //send light values to shaders
    for(auto* program : lit_programs){
        glUseProgram(program->program_uint);
        glUniform4fv(program->ambient_intensity_unif, 1, glm::value_ptr(ambient_light.intensity));
        glUniform4fv(program->light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
        glUniform1f(program->light_attenuation_unif, point_light.attenuation);
        glUseProgram(0);
    }
}

void initializeBuffers(){
    gltf_util::Loader loader;

    gltf_util::Model sphere = loader.loadModel("sphere_smooth.glb");
    gltf_util::Model plane = loader.loadModel("plane04.glb");

    sphere_data = core_util::loadModelData(sphere);
    plane_data = core_util::loadModelData(plane);
}

void initializeVertexArrayObjects(){
    sphere_vao = core_util::loadVAOData(sphere_data);
    plane_vao = core_util::loadVAOData(plane_data);
}

void initializeCameras(GLFWwindow* window){
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    cam = std::make_unique<Camera>(width,height);
    cam_controler = std::make_unique<CameraController>(*cam);
    cam->position = glm::vec3(0.0f, 0.0f, 2.5f);
    cam_controler->movement_speed = 3.0f;
    camera_movement_flag = true;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void initializeIMGUI(GLFWwindow* window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void init(GLFWwindow* window){
    initializePrograms();
    initializeUBOs();
    initializeNodes();
    initializeLights();
    initializeBuffers();
    initializeVertexArrayObjects();
    initializeCameras(window);
    initializeIMGUI(window);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    viewport_fb = std::make_unique<Framebuffer>(width,height);

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// Called every frame
void display(GLFWwindow* window){
    glfwPollEvents();

    //delta time calcs
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    //camera movement
    if(camera_movement_flag){
        cam_controler->processCameraInput(window, delta_time);
        cam->updateCamera();
    }

    //bulb movement
    bulb_controller.halfRotatePointLight(current_frame, delta_time);
    bulb_controller.processPointLightInput(window, delta_time);

    //program switching
    switch(light_model){
        case LM_LAMBERTIAN:
            current_program = &lambertian_program;
            ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        case LM_PHONG_LIGHTING:
            current_program = &phong_program;
            nodes["sphere"].material.shininess_factor = 32.0f;
            nodes["plane"].material.shininess_factor = 16.0f;
            ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
            break;
        case LM_BLINN_LIGHTING:
            current_program = &blinn_program;
            nodes["sphere"].material.shininess_factor = 128.0f;
            nodes["plane"].material.shininess_factor = 64.0f;
            ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
            break;
        case LM_GAUSSIAN_LIGHTING:
            current_program = &gaussian_program;
            nodes["sphere"].material.shininess_factor = 0.15f;
            nodes["plane"].material.shininess_factor = 0.4f;
            ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); 
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
            break;
        case LM_PBR_LIGHTING:
            ambient_light.intensity = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
            point_light.intensity = point_light.intensity = glm::vec4(1.0f, 1.0, 1.0f, 1.0f);
            current_program = &pbr_program;
            break;
    }

    //==========
    //DEAR IMGUI 
    //==========
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    renderGUI::ImGuiDemoDockspaceArgs args;
    renderGUI::dockingDemo(&args, nullptr);
    renderGUI::renderNodeWindow(nodes, ambient_light, point_light);

    ImGui::Begin("Viewport");
    // Get the size of the content region
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();
    
    // Resize framebuffer if needed
    if (viewport_size.x != viewport_fb->width || viewport_size.y != viewport_fb->height) {
        viewport_fb->Resize((int)viewport_size.x, (int)viewport_size.y);
    }

    cam->viewport_w = (int)viewport_size.x;
    cam->viewport_h = (int)viewport_size.y;
    cam->updatePerspMat();

    viewport_fb->Bind();

    //==========
    //Start Here
    //==========
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //==============
    //LIGHT UNIFORMS
    //==============
    glUseProgram(current_program->program_uint);

    glUniform4fv(current_program->ambient_intensity_unif, 1, glm::value_ptr(ambient_light.intensity));
    glUniform4fv(current_program->light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
    glUniform1f(current_program->light_attenuation_unif, point_light.attenuation);

    //=============
    //RENDER SPHERE 
    //=============
    glm::mat4 sphere_mv_mat = cam->getViewMat() * nodes["sphere"].transform.model_mat;
    glm::vec4 p_light_camera_pos = cam->getViewMat() * glm::vec4(point_light.position, 1.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(sphere_mv_mat));

    glUniform4fv(current_program->material_diffuse_unif, 1, glm::value_ptr(nodes["sphere"].material.diffuse_color));
    glUniform3fv(current_program->camera_space_light_position_unif, 1, glm::value_ptr(glm::vec3(p_light_camera_pos)));
    glUniform1f(current_program->shininess_factor_unif, nodes["sphere"].material.shininess_factor);

    //pbr
    glUniform4fv(current_program->base_color_unif, 1, glm::value_ptr(nodes["sphere"].material.base_color));
    glUniform1f(current_program->metallic_unif, nodes["sphere"].material.metallic);
    glUniform1f(current_program->roughness_unif, nodes["sphere"].material.roughness);

    glBindVertexArray(sphere_vao.vao);
	glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    //============
    //RENDER PLANE 
    //============
    glm::mat4 plane_mv_mat = cam->getViewMat() * nodes["plane"].transform.model_mat;
    
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(plane_mv_mat));

    glUniform4fv(current_program->material_diffuse_unif, 1, glm::value_ptr(nodes["plane"].material.diffuse_color));
    glUniform1f(current_program->shininess_factor_unif, nodes["plane"].material.shininess_factor);

    //pbr
    glUniform4fv(current_program->base_color_unif, 1, glm::value_ptr(nodes["plane"].material.base_color));
    glUniform1f(current_program->metallic_unif, nodes["plane"].material.metallic);
    glUniform1f(current_program->roughness_unif, nodes["plane"].material.roughness);

    glBindVertexArray(plane_vao.vao);
    glDrawElements(GL_TRIANGLES, plane_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    //===========
    //RENDER BULB
    //===========
    glUseProgram(unlit_program.program_uint);

    nodes["bulb"].material.diffuse_color = point_light.intensity;
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb"].material.diffuse_color));

    nodes["bulb"].transform.translation_component = point_light.position;
    nodes["bulb"].transform.calc_model_mat();

    glm::mat4 bulb_mv_mat = cam->getViewMat() * nodes["bulb"].transform.model_mat;

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb_mv_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    if(bulb_controller.draw_flag){
        glBindVertexArray(sphere_vao.vao);
        glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

    viewport_fb->Unbind();
    // Display the framebuffer texture in ImGui
    ImGui::Image(
        (void*)(intptr_t)viewport_fb->textureID,
        viewport_size,
        ImVec2(0, 1),  // UV coordinates (flipped vertically)
        ImVec2(1, 0)
    );
    ImGui::End();

    //=================
    //RENDER DEAR IMGUI 
    //=================
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    cam->viewport_w = width;
    cam->viewport_h = height;
    cam->updatePerspMat();

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    bool rotation_changed = false;
    bool light_draw_changed = false;
    bool light_model_changed = false;

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        bulb_controller.rotate_flag = !bulb_controller.rotate_flag;
        rotation_changed = true;
    }

    if(key == GLFW_KEY_E && action == GLFW_PRESS){
        bulb_controller.draw_flag = !bulb_controller.draw_flag;

        if(bulb_controller.draw_flag){
            point_light.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else{
            point_light.intensity = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        light_draw_changed = true;
    }

    if(key == GLFW_KEY_Q && action == GLFW_PRESS){
        light_model += 1;
        light_model %= LM_COUNT;
        light_model_changed = true;
    }

    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
        camera_movement_flag = !camera_movement_flag;

        if(camera_movement_flag){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cam_controler->first_mouse = true;
        }
    }

    //console logging
    if(rotation_changed){
        bulb_controller.rotate_flag ? std::cout << "Light Rotation: On\n" : std::cout << "Light Rotation: Off\n";
    }
    if(light_draw_changed){
        bulb_controller.draw_flag ? std::cout << "Point Light: On\n" : std::cout << "Point Light: Off\n";
    }
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    if(camera_movement_flag)
        cam_controler->mouseCameraController(window, x_pos, y_pos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    if(camera_movement_flag)
        cam_controler->mouseZoomController(window, x_offset, y_offset);
}

void cleanup(){
    for(auto* program : lit_programs){
        glDeleteProgram(program->program_uint);
    }
    glDeleteProgram(unlit_program.program_uint);

    core_util::cleanupBuffers(sphere_data);
    core_util::cleanupBuffers(plane_data);

    glDeleteVertexArrays(1, &sphere_vao.vao);
    glDeleteVertexArrays(1, &plane_vao.vao);

    glDeleteBuffers(1, &matrices_UBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


