//=============== switchfl1p 2025-2026 =======================

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <tiny_gltf.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <memory>

#include <shader_util.hpp>
#include <gltf_util.hpp>
#include <imgui_util.hpp>
#include <gl_util.hpp>

#include <CameraController.hpp>
#include <LightManager.hpp>
#include <Node.hpp>
#include "light_config.hpp"

//=============================================================

std::vector<shader_util::LitProgramData> lit_programs;
shader_util::UnlitProgramData unlit_program;

//for switching between programs
shader_util::LitProgramData* current_program;
shader_util::LitProgramData* current_terrain_program;

GLuint matrices_UBO;
GLuint lights_UBO;
GLuint materials_UBO;

int light_model = LM_GAUSSIAN_LIGHTING;

std::unordered_map<std::string, Node> nodes;
std::unique_ptr<TerrainData> terrain = nullptr;

gl_util::ModelData plane_data;
gl_util::VAOData plane_vao;
gl_util::ModelData sphere_data;
gl_util::VAOData sphere_vao;

std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controller = nullptr;
bool camera_movement_flag = true;
bool point_light_movement_flag = true;
bool sun_movement_flag = true;

float delta_time = 0.0f;
float last_frame = 0.0f;

std::unique_ptr<gl_util::Framebuffer> viewport_fb = nullptr;

std::unique_ptr<LightManager> light_manager = nullptr;

float g_gamma = 1.0f/2.2f;
glm::vec4 g_gamma_vec = glm::vec4(glm::vec3(g_gamma),1.0f);

//=============================================================

void initializePrograms(){
    std::unordered_map<std::string, std::string> defines = {
    {"MAX_POINT_LIGHTS", std::to_string(light_config::NUMBER_OF_POINT_LIGHTS)},
    {"MAX_DIR_LIGHTS", std::to_string(light_config::NUMBER_OF_DIR_LIGHTS)}
    };

    shader_util::LitProgramData lambertian_program = shader_util::loadLitProgram("pass_normals.vert", "lambertian.frag", defines);
    shader_util::LitProgramData phong_program = shader_util::loadLitProgram("pass_normals.vert", "phong.frag", defines);
    shader_util::LitProgramData blinn_program = shader_util::loadLitProgram("pass_normals.vert", "blinn_phong.frag", defines);
    shader_util::LitProgramData gaussian_program = shader_util::loadLitProgram("pass_normals.vert", "gaussian.frag", defines);
    shader_util::LitProgramData pbr_program = shader_util::loadLitProgram("pass_normals.vert", "pbr.frag", defines);

    //terrain
    shader_util::LitProgramData terrain_pbr_program = shader_util::loadLitProgram("pass_heights.vert", "terrain_pbr.frag", defines);
    shader_util::LitProgramData terrain_lambertian_program = shader_util::loadLitProgram("pass_heights.vert", "terrain_lambertian.frag", defines);
    shader_util::LitProgramData terrain_phong_program = shader_util::loadLitProgram("pass_heights.vert", "terrain_phong.frag", defines);
    shader_util::LitProgramData terrain_gaussian_program = shader_util::loadLitProgram("pass_heights.vert", "terrain_gaussian.frag", defines);
    shader_util::LitProgramData terrain_blinn_program = shader_util::loadLitProgram("pass_heights.vert", "terrain_blinn.frag", defines);
    
    //No Light
    unlit_program = shader_util::loadUnlitProgram("simple.vert", "no_light.frag");

    lit_programs.reserve(LM_COUNT * 2);
    lit_programs.push_back(lambertian_program);
    lit_programs.push_back(phong_program);
    lit_programs.push_back(blinn_program);
    lit_programs.push_back(gaussian_program);
    lit_programs.push_back(pbr_program);

    lit_programs.push_back(terrain_lambertian_program);
    lit_programs.push_back(terrain_phong_program);
    lit_programs.push_back(terrain_blinn_program);
    lit_programs.push_back(terrain_gaussian_program);
    lit_programs.push_back(terrain_pbr_program);
}

void initializeUBOs(){
    GLuint matrices_uniform_block_index;
    GLuint lights_uniform_block_index;
    GLuint materials_uniform_block_index;
    constexpr int matrices_binding_index = 0;
    constexpr int lights_binding_index = 1;
    constexpr int materials_binding_index = 2;

    //Matrices UBO
    //Create UBO and bind to binding index
    glGenBuffers(1, &matrices_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, matrices_binding_index, matrices_UBO, 0, sizeof(glm::mat4) * 3);

    //Lights UBO
    glGenBuffers(1, &lights_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlock), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, lights_binding_index, lights_UBO, 0, sizeof(LightBlock));

    //Materials UBO
    glGenBuffers(1, &materials_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, materials_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialBlock), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, materials_binding_index, materials_UBO, 0, sizeof(MaterialBlock));

    //handle lit programs
    for(auto& program : lit_programs){
        matrices_uniform_block_index = glGetUniformBlockIndex(program.program_uint, "Matrices");
        glUniformBlockBinding(program.program_uint, matrices_uniform_block_index, matrices_binding_index);
        
        lights_uniform_block_index = glGetUniformBlockIndex(program.program_uint, "Lights");
        glUniformBlockBinding(program.program_uint, lights_uniform_block_index, lights_binding_index);

        materials_uniform_block_index = glGetUniformBlockIndex(program.program_uint, "Materials");
        glUniformBlockBinding(program.program_uint, materials_uniform_block_index, materials_binding_index);
    }

    //handle unlit ones
    matrices_uniform_block_index = glGetUniformBlockIndex(unlit_program.program_uint, "Matrices");
    glUniformBlockBinding(unlit_program.program_uint, matrices_uniform_block_index, matrices_binding_index);
}

void initializeNodes(){
    Node bulb;
    bulb.transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
    bulb.material.is_emissive = true;
    nodes["bulb_1"] = bulb;

    Node bulb_2;
    bulb_2.transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
    bulb_2.material.is_emissive = true;
    nodes["bulb_2"] = bulb_2;

    Node bulb_3;
    bulb_3.transform.scale_component = glm::vec3(0.05f, 0.05f, 0.05f);
    bulb_3.material.is_emissive = true;
    nodes["bulb_3"] = bulb_3;

    Node sun;
    sun.transform.scale_component = glm::vec3(0.5f, 0.5f, 0.5f);
    sun.material.is_emissive = true;
    nodes["sun"] = sun;

    Node moon;
    moon.transform.scale_component = glm::vec3(0.5f, 0.5f, 0.5f);
    moon.material.is_emissive = true;
    nodes["moon"] = moon;

    Node plane;
    plane.material.classic.shininess = 64.0f;
    plane.material.classic.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    plane.material.classic.specular_color = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    plane.material.pbr.color = glm::vec4(0.972f, 0.960f, 0.915f, 1.0f);
    plane.material.pbr.metallic = 0.0f;
    plane.material.pbr.roughness = 0.7f;
    plane.transform.translation_component = glm::vec3(0.0f, -2.0f, 0.0f);
    plane.transform.scale_component = glm::vec3(25.0f, 1.0f, 25.0f);

    plane.transform.calc_model_mat();
    nodes["plane"] = plane;
}

void initializeLights(){
    light_config::PointLightsData pl_data = light_config::initPointLights();
    light_manager = std::make_unique<LightManager>(pl_data.pl_interpolators, pl_data.pl_timers);
    light_config::setupLighValues(*light_manager, light_config::LIGHT_PBR);
    light_manager->togglePause(TIMER_SUN);
}

void initializeBuffers(){
    gltf_util::Loader loader;

    gltf_util::Model plane = loader.loadModel("plane.glb");
    plane_data = gl_util::createBuffers(plane);

    gltf_util::Model sphere = loader.loadModel("sphere_smooth.glb");
    sphere_data = gl_util::createBuffers(sphere);
}

void initializeVertexArrayObjects(){
    plane_vao = gl_util::createVAO(plane_data);
    sphere_vao = gl_util::createVAO(sphere_data);
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
    ImGui::StyleColorsDark();
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
    viewport_fb = std::make_unique<gl_util::Framebuffer>(width,height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// Called every frame
void display(GLFWwindow* window){
    light_manager->updateTime();
    LightBlock light_block = light_manager->getLightInformation(cam->getViewMat());
    light_block.gamma = g_gamma;

    glm::vec4 bkg = light_manager->getBackgroundColor();
    bkg = glm::pow(bkg, g_gamma_vec); //gamma correction

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

    PointLight& point_light = light_block.point_lights[0];
    PointLight& point_light_2 = light_block.point_lights[1];
    PointLight& point_light_3 = light_block.point_lights[2];

    MaterialBlock material_block;

    static int previous_light_model = -1;
    if(light_model != previous_light_model){

        switch(light_model){
            case LM_LAMBERTIAN:
                current_program = &lit_programs[LM_LAMBERTIAN];
                light_config::setupLighValues(*light_manager, light_config::LIGHT_CLASSIC);
                break;
            case LM_PHONG_LIGHTING:
                current_program = &lit_programs[LM_PHONG_LIGHTING];
                nodes["plane"].material.classic.shininess = 16.0f;
                break;
            case LM_BLINN_LIGHTING:
                current_program = &lit_programs[LM_BLINN_LIGHTING];
                nodes["plane"].material.classic.shininess = 64.0f;
                break;
            case LM_GAUSSIAN_LIGHTING:
                current_program = &lit_programs[LM_GAUSSIAN_LIGHTING];
                nodes["plane"].material.classic.shininess = 0.4f;
                break;
            case LM_PBR_LIGHTING:
                current_program = &lit_programs[LM_PBR_LIGHTING];
                light_config::setupLighValues(*light_manager, light_config::LIGHT_PBR);
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
    
    imgui_util::ImGuiDemoDockspaceArgs args;
    imgui_util::dockingDemo(&args, nullptr);
    imgui_util::renderNodeWindow(nodes, light_model);
    imgui_util::renderLightWindow(light_block.ambient_light, light_block.point_lights, light_block.dir_lights);
    imgui_util::renderStatusOverlay(light_model, sun_movement_flag, point_light_movement_flag, camera_movement_flag);

    ImGui::Begin("Viewport");
    // Get the size of the content region
    ImVec2 viewport_size = ImGui::GetContentRegionAvail();

    cam->viewport_w = (int)viewport_size.x;
    cam->viewport_h = (int)viewport_size.y;
    cam->updatePerspMat();

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cam->getViewMat()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::mat4), glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    viewport_fb->Bind();

    //==========
    //Start Here
    //==========
    glClearColor(bkg[0], bkg[1], bkg[2], bkg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //=============
    //RENDER PLANE
    //=============
    glUseProgram(current_program->program_uint);

    //Lights
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBlock), &light_block);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //modelToWorld Matrix
    glm::mat4 plane_model_mat = nodes["plane"].transform.model_mat;
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(plane_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //material properties
    material_block.getMaterialInformation(nodes["plane"]);
    glBindBuffer(GL_UNIFORM_BUFFER, materials_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialBlock), &material_block);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //draw
    glBindVertexArray(plane_vao.vao);
	glDrawElements(GL_TRIANGLES, plane_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    
    glUseProgram(0);

    //============
    //RENDER BULBS
    //============
    //1
    glUseProgram(unlit_program.program_uint);

    nodes["bulb_1"].material.classic.color = glm::pow(point_light.intensity, g_gamma_vec);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb_1"].material.classic.color));

    nodes["bulb_1"].transform.translation_component = light_manager->getWorldLightPosition(0);
    nodes["bulb_1"].transform.calc_model_mat();

    glm::mat4 bulb_model_mat = nodes["bulb_1"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindVertexArray(sphere_vao.vao);
    glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    //2
    nodes["bulb_2"].material.classic.color = glm::pow(point_light_2.intensity, g_gamma_vec);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb_2"].material.classic.color));

    nodes["bulb_2"].transform.translation_component = light_manager->getWorldLightPosition(1);
    nodes["bulb_2"].transform.calc_model_mat();

    glm::mat4 bulb2_model_mat = nodes["bulb_2"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb2_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(sphere_vao.vao);
    glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    //3
    nodes["bulb_3"].material.classic.color = glm::pow(point_light_3.intensity, g_gamma_vec);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["bulb_3"].material.classic.color));

    nodes["bulb_3"].transform.translation_component = light_manager->getWorldLightPosition(2);
    nodes["bulb_3"].transform.calc_model_mat();

    glm::mat4 bulb3_model_mat = nodes["bulb_3"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(bulb3_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(sphere_vao.vao);
    glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    //==========
    //RENDER SUN
    //==========
    glUseProgram(unlit_program.program_uint);
    nodes["sun"].material.classic.color = glm::pow(light_block.dir_lights[0].intensity, g_gamma_vec);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["sun"].material.classic.color));

    nodes["sun"].transform.translation_component = glm::vec3(light_manager->getSunlightDirection()) * 16.0f;
    nodes["sun"].transform.calc_model_mat();

    glm::mat4 sun_model_mat = nodes["sun"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(sun_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindVertexArray(sphere_vao.vao);
    glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    //===========
    //RENDER MOON
    //===========
    glUseProgram(unlit_program.program_uint);
    nodes["moon"].material.classic.color = glm::pow(light_block.dir_lights[1].intensity, g_gamma_vec);
    glUniform4fv(unlit_program.material_diffuse_unif, 1, glm::value_ptr(nodes["moon"].material.classic.color));

    nodes["moon"].transform.translation_component = -glm::vec3(light_manager->getSunlightDirection()) * 16.0f;
    nodes["moon"].transform.calc_model_mat();

    glm::mat4 moon_model_mat = nodes["moon"].transform.model_mat;

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(moon_model_mat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindVertexArray(sphere_vao.vao);
    glDrawElements(GL_TRIANGLES, sphere_vao.index_count, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    //=================
    //RENDER DEAR IMGUI 
    //=================
    viewport_fb->Unbind();
    //Display the framebuffer texture in ImGui
    ImGui::Image(
        (void*)(intptr_t)viewport_fb->textureID,
        viewport_size,
        ImVec2(0, 1),  // UV coordinates (flipped vertically)
        ImVec2(1, 0)
    );
    ImGui::End();

    //ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    cam->viewport_w = width;
    cam->viewport_h = height;
    cam->updatePerspMat();

    glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4) * 2, glm::value_ptr(cam->getPerspMat()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    //Exit
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    //Toggle Camera lock On/Off
    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
        camera_movement_flag = !camera_movement_flag;

        if(camera_movement_flag){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cam_controller->first_mouse = true;
        }
    }

    //Toggle Point Light movement On/Off
    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        light_manager->togglePause(TIMER_LIGHTS);
        point_light_movement_flag = !point_light_movement_flag;
    }

    //Toggle sunlight timer pause on/off
    if(key == GLFW_KEY_T && action == GLFW_PRESS){
        light_manager->togglePause(TIMER_SUN);
        sun_movement_flag = !sun_movement_flag;
    }

    //Lighting model switching
    if(key == GLFW_KEY_Q && action == GLFW_PRESS){
        light_model += 1;
        light_model %= LM_COUNT;
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

    for(auto& program : lit_programs){
        glDeleteProgram(program.program_uint);
    }
    glDeleteProgram(unlit_program.program_uint);

    viewport_fb.reset(); //needs to be here or segfault on exit

    gl_util::cleanupBuffers(plane_data);
    glDeleteVertexArrays(1, &plane_vao.vao);

    glDeleteBuffers(1, &matrices_UBO);
    glDeleteBuffers(1, &lights_UBO);
    glDeleteBuffers(1, &materials_UBO);
}


