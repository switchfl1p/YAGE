//=============== switchfl1p 2025-2026 ==================

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <memory>
#include <tiny_gltf.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <iostream>

#include <Camera.hpp>
#include <CameraController.hpp>
#include <gltf_util.hpp>
#include <Light.hpp>
#include <LightController.hpp>
#include <Node.hpp>
#include <core_util.hpp>
#include <renderGUI.hpp>
#include <Terrain.hpp>

//========================================================

std::vector<core_util::LitProgramData*> lit_programs;
core_util::LitProgramData lambertian_program;
core_util::LitProgramData phong_program;
core_util::LitProgramData blinn_program;
core_util::LitProgramData gaussian_program;
core_util::LitProgramData pbr_program;

core_util::LitProgramData terrain_pbr_program;
core_util::LitProgramData terrain_lambertian_program;
core_util::LitProgramData terrain_phong_program;
core_util::LitProgramData terrain_gaussian_program;
core_util::LitProgramData terrain_blinn_program;

core_util::UnlitProgramData unlit_program;

//program switching helper
core_util::LitProgramData* current_program;
core_util::LitProgramData* current_terrain_program;

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
constexpr int matrices_binding_index = 0;

GLuint lights_uniform_block_index;
GLuint lights_UBO;
constexpr int lights_binding_index = 1;

std::unordered_map<std::string, Node> nodes;

AmbientLight ambient_light;
//std::vector<DirectionalLight> directional_lights;
//std::vector<PointLight> point_lights;
std::unique_ptr<LightController> bulb_controller;
std::unique_ptr<LightController> bulb2_controller;

constexpr int MAX_POINT_LIGHTS = 2;
constexpr int MAX_DIR_LIGHTS = 2;

struct LightBuffer{
    PointLight point_lights[MAX_POINT_LIGHTS];
    DirectionalLight dir_lights[MAX_DIR_LIGHTS];
    int point_light_count;
    int dir_light_count;
};

LightBuffer light_buffer;
LightBuffer light_buffer_GPU;

core_util::ModelData sphere_data;
core_util::ModelData plane_data;

core_util::VAOData sphere_vao;
core_util::VAOData plane_vao;

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controller = nullptr;
bool camera_movement_flag;

float delta_time = 0.0f;
float last_frame = 0.0f;

std::unique_ptr<core_util::Framebuffer> viewport_fb = nullptr;
std::unique_ptr<TerrainData> terrain = nullptr;

//========================================================

void initializePrograms(){
    lambertian_program = core_util::loadLitProgram("pass_normals.vert", "lambertian.frag");
    phong_program = core_util::loadLitProgram("pass_normals.vert", "phong.frag");
    blinn_program = core_util::loadLitProgram("pass_normals.vert", "blinn_phong.frag");
    gaussian_program = core_util::loadLitProgram("pass_normals.vert", "gaussian.frag");
    pbr_program = core_util::loadLitProgram("pass_normals.vert", "pbr.frag");

    //terrain
    terrain_pbr_program = core_util::loadLitProgram("pass_heights.vert", "terrain_pbr.frag");
    terrain_lambertian_program = core_util::loadLitProgram("pass_heights.vert", "terrain_lambertian.frag");
    terrain_phong_program = core_util::loadLitProgram("pass_heights.vert", "terrain_phong.frag");
    terrain_gaussian_program = core_util::loadLitProgram("pass_heights.vert", "terrain_gaussian.frag");
    terrain_blinn_program = core_util::loadLitProgram("pass_heights.vert", "terrain_blinn.frag");
    
    //No Light
    unlit_program = core_util::loadUnlitProgram("simple.vert", "no_light.frag");

    lit_programs.push_back(&lambertian_program);
    lit_programs.push_back(&phong_program);
    lit_programs.push_back(&blinn_program);
    lit_programs.push_back(&gaussian_program);
    lit_programs.push_back(&pbr_program);

    lit_programs.push_back(&terrain_pbr_program);
    lit_programs.push_back(&terrain_lambertian_program);
    lit_programs.push_back(&terrain_phong_program);
    lit_programs.push_back(&terrain_gaussian_program);
    lit_programs.push_back(&terrain_blinn_program);
}

void initializeUBOs(){
    //Matrices UBO
    //Create UBO and bind to binding index
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 3);

    //handle lit programs
    for(auto* program : lit_programs){
        matrices_uniform_block_index = glGetUniformBlockIndex(program->program_uint, "Matrices");
        glUniformBlockBinding(program->program_uint, matrices_uniform_block_index, matrices_binding_index);
    }

    //handle unlit ones
    matrices_uniform_block_index = glGetUniformBlockIndex(unlit_program.program_uint, "Matrices");
    glUniformBlockBinding(unlit_program.program_uint, matrices_uniform_block_index, matrices_binding_index);

    //Lights UBO
    glGenBuffers(1, &lights_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, lights_binding_index, lights_UBO, 0, sizeof(LightBuffer));

    //handle lit programs
    for(auto* program : lit_programs){
        lights_uniform_block_index = glGetUniformBlockIndex(program->program_uint, "Lights");
        glUniformBlockBinding(program->program_uint, lights_uniform_block_index, lights_binding_index);
    }
}

void initializeNodes(){
    //Bulb
    {
        Node bulb;
        Material bulb_material;
        bulb_material.type = Material::EMISSIVE;
        bulb_material.phong_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //white
        bulb.material = bulb_material;

        Transform bulb_transform;
        bulb_transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
        bulb.transform = bulb_transform;

        nodes["bulb"] = bulb;
    }
        //2nd Bulb
    {
        Node bulb_2;
        Material bulb_material;
        bulb_material.type = Material::EMISSIVE;
        bulb_material.phong_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); //white
        bulb_2.material = bulb_material;

        Transform bulb_transform;
        bulb_transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
        bulb_2.transform = bulb_transform;

        nodes["bulb_2"] = bulb_2;
    }

    //Centerpiece
    {
        Node sphere;
        Material sphere_material;
        sphere_material.phong_color = glm::vec4(0.5, 0.1, 0.8, 1.0);  //blue
        sphere_material.shininess = 64.0f;

        //pbr
        sphere_material.pbr_color = glm::vec4(0.3, 0.0, 0.7, 1.0);
        sphere_material.metallic = 0.0f;   //plastic/ceramic
        sphere_material.roughness = 0.30f;  //somewhat shiny
        sphere.material = sphere_material;

        Transform sphere_transform;
        sphere_transform.scale_component = glm::vec3(0.5f, 0.5f, 0.5f);
        sphere_transform.calc_model_mat();
        sphere.transform = sphere_transform;

        nodes["sphere"] = sphere;
    }

    //Plane (now terrain)
    {
        Node plane;
        Material plane_material;
        plane_material.phong_color = glm::vec4(0.5, 0.5, 0.5, 1.0); //silver
        plane_material.shininess = 64.0f;

        //pbr
        plane_material.pbr_color = glm::vec4(0.8, 0.8, 0.8, 1.0);
        plane_material.metallic = 0.0f;
        plane_material.roughness = 0.7f;  //matte
        plane.material = plane_material;

        Transform plane_transform;
        plane_transform.translation_component = glm::vec3(-25.0f,0.0,-25.0f);
        plane_transform.calc_model_mat();
        plane.transform = plane_transform;

        nodes["plane"] = plane;
    }
}

void initializeLights(){
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    ambient_light.intensity = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
    
    PointLight pl;
    pl.intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    pl.attenuation = 1.0f;
    pl.position = glm::vec4(0.0f, 0.25f, 0.0f, 1.0f);

    PointLight pl_2;
    pl_2.intensity = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    pl_2.attenuation = 1.0f;
    pl_2.position = glm::vec4(0.0f, 2.0f, 0.0f, 1.0f);
    
    point_lights.push_back(pl);
    point_lights.push_back(pl_2);

    DirectionalLight sun;
    sun.direction = glm::vec4(-0.3f, -1.0f, -0.4f, 0.0f); //slightly angled, not straight down
    sun.intensity = glm::vec4(1.0f, 0.95f, 0.8f, 1.0f); //warm white

    directional_lights.push_back(sun);

    //copy to LightBuffer for use with UBO
    light_buffer.point_light_count = point_lights.size();
    light_buffer.dir_light_count = directional_lights.size();

    for(int i = 0; i < point_lights.size(); i++){
        light_buffer.point_lights[i] = point_lights[i];
    }

    for(int i = 0; i < directional_lights.size(); i++){
        light_buffer.dir_lights[i] = directional_lights[i];
    }

    bulb_controller = std::make_unique<LightController>(light_buffer.point_lights[0]);
    bulb_controller->radius = 1.0f;

    bulb2_controller = std::make_unique<LightController>(light_buffer.point_lights[1]);
    bulb2_controller->radius = 2.0f;

    light_buffer_GPU = light_buffer;
}

void initializeBuffers(){
    gltf_util::Loader loader;

    gltf_util::Model sphere = loader.loadModel("sphere_smooth.glb");

    terrain = std::make_unique<TerrainData>(50, 50, 5.76f , 16, 10.94f , 1, 6, 2.0, 0.5);

    sphere_data = core_util::loadModelData(sphere);
    plane_data = core_util::createTerrainBuffers(*terrain);
}

void initializeVertexArrayObjects(){
    sphere_vao = core_util::loadVAOData(sphere_data);
    plane_vao = core_util::createTerrainVAO(plane_data);
}

void initializeCameras(GLFWwindow* window){
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    cam = std::make_unique<Camera>(width,height);
    cam_controller = std::make_unique<CameraController>(*cam);
    cam->position = glm::vec3(0.0f, 0.0f, 2.5f);
    cam_controller->movement_speed = 3.0f;
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
    viewport_fb = std::make_unique<core_util::Framebuffer>(width,height);

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
        cam_controller->processCameraInput(window, delta_time);
        cam->updateCamera();
    }

    //bulb movement
    bulb_controller->rotatePointLight(current_frame, delta_time);
    bulb_controller->processPointLightInput(window, delta_time);

    bulb2_controller->halfRotatePointLight(current_frame, delta_time);
    bulb2_controller->processPointLightInput(window, delta_time);

    PointLight& point_light = light_buffer.point_lights[0];
    PointLight& point_light_2 = light_buffer.point_lights[1];

    static int previous_light_model = -1;
    if(light_model != previous_light_model){

        switch(light_model){
            case LM_LAMBERTIAN:
                current_program = &lambertian_program;
                current_terrain_program = &terrain_lambertian_program;
                nodes["sphere"].material.type = Material::LAMBERTIAN;
                nodes["plane"].material.type= Material::LAMBERTIAN;
                ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
                break;
            case LM_PHONG_LIGHTING:
                current_program = &phong_program;
                current_terrain_program = &terrain_phong_program;
                nodes["sphere"].material.type = Material::PHONG;
                nodes["plane"].material.type = Material::PHONG;
                nodes["sphere"].material.shininess = 32.0f;
                nodes["plane"].material.shininess = 16.0f;
                ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
                break;
            case LM_BLINN_LIGHTING:
                current_program = &blinn_program;
                current_terrain_program = &terrain_blinn_program;
                nodes["sphere"].material.type = Material::PHONG;
                nodes["plane"].material.type = Material::PHONG;
                nodes["sphere"].material.shininess = 128.0f;
                nodes["plane"].material.shininess = 64.0f;
                ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
                break;
            case LM_GAUSSIAN_LIGHTING:
                current_program = &gaussian_program;
                current_terrain_program = &terrain_gaussian_program;
                nodes["sphere"].material.type = Material::PHONG;
                nodes["plane"].material.type = Material::PHONG;
                nodes["sphere"].material.shininess = 0.15f;
                nodes["plane"].material.shininess = 0.4f;
                ambient_light.intensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); 
                break;
            case LM_PBR_LIGHTING:
                current_program = &pbr_program;
                current_terrain_program = &terrain_pbr_program;
                nodes["sphere"].material.type = Material::PBR;
                nodes["plane"].material.type = Material::PBR;
                ambient_light.intensity = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
                break;
        }
        previous_light_model = light_model;
    }

    //==========
    //DEAR IMGUI 
    //==========
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    renderGUI::ImGuiDemoDockspaceArgs args;
    renderGUI::dockingDemo(&args, nullptr);
    renderGUI::renderNodeWindow(nodes);
    renderGUI::renderLightWindow(ambient_light, light_buffer.point_lights);
    renderGUI::renderStatusOverlay(light_model, bulb_controller->draw_flag, bulb_controller->rotate_flag, camera_movement_flag);

    if(renderGUI::renderTerrainWindow(*terrain)){
        terrain->generateTerrain();
        core_util::cleanupBuffers(plane_data);
        glDeleteVertexArrays(1, &plane_vao.vao);
        plane_data = core_util::createTerrainBuffers(*terrain);
        plane_vao = core_util::createTerrainVAO(plane_data);
    }

    ImGui::Begin("Viewport");
    // Get the size of the content region
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();

    cam->viewport_w = (int)viewport_size.x;
    cam->viewport_h = (int)viewport_size.y;
    cam->updatePerspMat();

    viewport_fb->Bind();

    //==========
    //Start Here
    //==========
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //pass view and perspective matrix to UBO
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getViewMat()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //=============
    //RENDER SPHERE 
    //=============
    glUseProgram(current_program->program_uint);

    //light uniforms
    glUniform4fv(current_program->ambient_intensity_unif, 1, glm::value_ptr(ambient_light.intensity));
    glUniform4fv(current_program->light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
    glUniform1f(current_program->light_attenuation_unif, point_light.attenuation);
    glm::vec4 p_light_camera_pos = cam->getViewMat() * point_light.position;
    glUniform3fv(current_program->camera_space_light_position_unif, 1, glm::value_ptr(glm::vec3(p_light_camera_pos)));

    light_buffer_GPU = light_buffer;

    for(int i = 0; i < light_buffer.point_light_count; i++){
        light_buffer_GPU.point_lights[i].position = cam->getViewMat() * light_buffer.point_lights[i].position;
    }

    glm::mat3 view_rot = glm::mat3(cam->getViewMat());
    for(int i = 0; i < light_buffer.dir_light_count; i++){
        glm::vec3 dir = view_rot * light_buffer.dir_lights[i].direction;
        dir = glm::normalize(dir);
        light_buffer_GPU.dir_lights[i].direction = glm::vec4(dir, 0.0f);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &light_buffer_GPU);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //modeltoWorld uniform
    glm::mat4 sphere_model_mat = nodes["sphere"].transform.model_mat;
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(sphere_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //material uniforms
    //non-PBR
    glUniform4fv(current_program->material_diffuse_unif, 1, glm::value_ptr(nodes["sphere"].material.phong_color));
    glUniform1f(current_program->shininess_factor_unif, nodes["sphere"].material.shininess);
    //PBR
    glUniform4fv(current_program->base_color_unif, 1, glm::value_ptr(nodes["sphere"].material.pbr_color));
    glUniform1f(current_program->metallic_unif, nodes["sphere"].material.metallic);
    glUniform1f(current_program->roughness_unif, nodes["sphere"].material.roughness);

    //draw
    glBindVertexArray(sphere_vao.vao);
	glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    
    glUseProgram(0);
    

    //============
    //RENDER PLANE 
    //============
    glUseProgram(current_terrain_program->program_uint);

    glUniform4fv(current_terrain_program->ambient_intensity_unif, 1, glm::value_ptr(ambient_light.intensity));
    glUniform4fv(current_terrain_program->light_intensity_unif, 1, glm::value_ptr(point_light.intensity));
    glUniform1f(current_terrain_program->light_attenuation_unif, point_light.attenuation);
    glUniform3fv(current_terrain_program->camera_space_light_position_unif, 1, glm::value_ptr(glm::vec3(p_light_camera_pos)));

    glm::mat4 plane_model_mat = nodes["plane"].transform.model_mat;
    
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(plane_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUniform4fv(current_terrain_program->material_diffuse_unif, 1, glm::value_ptr(nodes["plane"].material.phong_color));
    glUniform1f(current_terrain_program->shininess_factor_unif, nodes["plane"].material.shininess);

    //pbr
    glUniform4fv(current_terrain_program->base_color_unif, 1, glm::value_ptr(nodes["plane"].material.pbr_color));
    glUniform1f(current_terrain_program->metallic_unif, nodes["plane"].material.metallic);
    glUniform1f(current_terrain_program->roughness_unif, nodes["plane"].material.roughness);

    //terrain
    glUniform1f(current_terrain_program->amplitutde_unif, terrain->amplitude);

    glBindVertexArray(plane_vao.vao);
    glDrawElements(GL_TRIANGLES, plane_vao.index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    //============
    //RENDER BULBS
    //============
    //1
    glUseProgram(unlit_program.program_uint);

    nodes["bulb"].material.phong_color = point_light.intensity;
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb"].material.phong_color));

    nodes["bulb"].transform.translation_component = point_light.position;
    nodes["bulb"].transform.calc_model_mat();

    glm::mat4 bulb_model_mat = nodes["bulb"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    if(bulb_controller->draw_flag){
        glBindVertexArray(sphere_vao.vao);
        glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

    //2
    nodes["bulb_2"].material.phong_color = point_light_2.intensity;
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb_2"].material.phong_color));

    nodes["bulb_2"].transform.translation_component = point_light_2.position;
    nodes["bulb_2"].transform.calc_model_mat();

    glm::mat4 bulb2_model_mat = nodes["bulb_2"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb2_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    if(bulb2_controller->draw_flag){
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
        bulb_controller->rotate_flag = !bulb_controller->rotate_flag;
        rotation_changed = true;
    }

    if(key == GLFW_KEY_E && action == GLFW_PRESS){
        bulb_controller->draw_flag = !bulb_controller->draw_flag;

        if(bulb_controller->draw_flag){
            light_buffer.point_lights[0].intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else{
            light_buffer.point_lights[0].intensity = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
            cam_controller->first_mouse = true;
        }
    }

    //console logging
    if(rotation_changed){
        bulb_controller->rotate_flag ? std::cout << "Light Rotation: On\n" : std::cout << "Light Rotation: Off\n";
    }
    if(light_draw_changed){
        bulb_controller->draw_flag ? std::cout << "Point Light: On\n" : std::cout << "Point Light: Off\n";
    }
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    if(camera_movement_flag)
        cam_controller->mouseCameraController(window, x_pos, y_pos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    if(camera_movement_flag)
        cam_controller->mouseZoomController(window, x_offset, y_offset);
}

void cleanup(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for(auto* program : lit_programs){
        glDeleteProgram(program->program_uint);
    }

    viewport_fb.reset(); //needs to be here or segfault on exit

    glDeleteProgram(unlit_program.program_uint);

    core_util::cleanupBuffers(sphere_data);
    core_util::cleanupBuffers(plane_data);

    glDeleteVertexArrays(1, &sphere_vao.vao);
    glDeleteVertexArrays(1, &plane_vao.vao);

    glDeleteBuffers(1, &matrices_UBO);
}


