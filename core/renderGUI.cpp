#include <renderGUI.hpp>
#include "LightManager.hpp"
#include "glm/gtc/type_ptr.hpp"

void renderGUI::renderNodeWindow(std::unordered_map<std::string, Node> &nodes, int light_model){
    ImGui::Begin("Node Options");
    
    for (auto& [name, node] : nodes) {
        //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode(name.c_str())) {
            if(!node.material.is_emissive){
                switch (light_model){
                    case LM_PBR_LIGHTING:
                        if(ImGui::TreeNode(("Material Properties##" + name).c_str())){
                            std::string base_color_label = "color##" + name;
                            std::string metallic_label = "metallic##" + name;
                            std::string roughness_label = "roughness##" + name;
                            
                            ImGui::ColorEdit4(base_color_label.c_str(), glm::value_ptr(node.material.pbr.color));
                            ImGui::DragFloat(metallic_label.c_str(), &node.material.pbr.metallic, 0.01f, 0.0f, 1.0f);
                            ImGui::DragFloat(roughness_label.c_str(), &node.material.pbr.roughness, 0.01f, 0.0f, 1.0f);
                            
                            ImGui::TreePop();
                        }
                        break;
                    case LM_LAMBERTIAN:
                        if(ImGui::TreeNode(("Material Properties##" + name).c_str())){
                            std::string base_color_label = "color##" + name;
                            
                            ImGui::ColorEdit4(base_color_label.c_str(), glm::value_ptr(node.material.classic.color));
                            
                            ImGui::TreePop();
                        }
                        break;
                    case LM_PHONG_LIGHTING:
                    case LM_BLINN_LIGHTING:
                    case LM_GAUSSIAN_LIGHTING:
                        if(ImGui::TreeNode(("Material Properties##" + name).c_str())){
                            std::string base_color_label = "color##" + name;
                            std::string shininess_label = "shininess##" + name;
                            
                            ImGui::ColorEdit4(base_color_label.c_str(), glm::value_ptr(node.material.classic.color));
                            ImGui::DragFloat(shininess_label.c_str(), &node.material.classic.shininess, 1.0f, 0.0f, 1000.0f);
                            
                            ImGui::TreePop();
                        }
                        break;
                }
            }

            if(ImGui::TreeNode(("Transform Properties##" + name).c_str())){
                std::string translation_label = "translation##" + name;
                std::string rotation_label = "rotation##" + name;
                std::string scale_label = "scale##" + name;
                
                bool transform_changed = false;
                transform_changed |= ImGui::DragFloat3(translation_label.c_str(), glm::value_ptr(node.transform.translation_component), 0.01f);
                transform_changed |= ImGui::DragFloat3(rotation_label.c_str(), glm::value_ptr(node.transform.rotation_component), 0.01f, -glm::two_pi<float>(), glm::two_pi<float>());
                transform_changed |= ImGui::DragFloat3(scale_label.c_str(), glm::value_ptr(node.transform.scale_component), 0.01f, 0.01f, 10.0f);
                
                if(transform_changed){
                    node.transform.calc_model_mat();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//span allows for use of vectors or arrays
void renderGUI::renderLightWindow(AmbientLight &ambient_light, std::span<PointLight> point_lights, std::span<DirectionalLight> dir_lights){
    ImGui::Begin("Light Options");

    if (ImGui::TreeNode("Ambient Light")) {
        ImGui::ColorEdit4("intensity##ambient", glm::value_ptr(ambient_light.intensity));
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Point Lights")) {
        for(size_t i = 0; i < point_lights.size(); i++){
            ImGui::PushID(i);
            std::string label = "Point Light " + std::to_string(i + 1);
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::ColorEdit4("intensity", glm::value_ptr(point_lights[i].intensity));
                ImGui::DragFloat("attenuation", &point_lights[i].attenuation, 0.01f, 0.0f, 5.0f);
                ImGui::DragFloat3("position", glm::value_ptr(point_lights[i].position));
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Directional Lights")) {
        for(size_t i = 0; i < dir_lights.size(); i++){
            ImGui::PushID(point_lights.size() + i);
            std::string label = "Directional Light " + std::to_string(i + 1);
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::ColorEdit4("intensity", glm::value_ptr(dir_lights[i].intensity));
                ImGui::DragFloat3("direction", glm::value_ptr(dir_lights[i].direction));
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void renderGUI::renderStatusOverlay(int light_model, bool light_status, bool rotation_status, bool camera_status){
    ImGui::Begin("Status");

    const char* lighting_names[] = {
        "Lambertian",
        "Phong",
        "Blinn-Phong",
        "Gaussian",
        "PBR"
    };

    ImGui::Text("Lighting Model: %s", lighting_names[light_model]);
    // Light status with color
    if (light_status) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Light: On");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Light: Off");
    }

    // Light rotation with color
    if (rotation_status) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Light Rotation: On");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Light Rotation: Off");
    }

    // Camera movement with color
    if (camera_status) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Camera Movement: On");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Camera Movement: Off");
    }
    ImGui::End();
}

bool renderGUI::renderTerrainWindow(TerrainData &terrain){
    bool terrain_changed = false;

    ImGui::Begin("Terrain Options");
    terrain_changed |= ImGui::DragInt("Width", &terrain.width, 1, 0, 1000);
    terrain_changed |= ImGui::DragInt("Depth", &terrain.depth, 1, 0, 1000);
    terrain_changed |= ImGui::SliderFloat("Scale", &terrain.scale, 0.0f, 100.0f);
    terrain_changed |= ImGui::InputInt("Seed", &terrain.seed);
    terrain_changed |= ImGui::SliderFloat("Amplitude", &terrain.amplitude, 0.0f, 35.0f);
    terrain_changed |= ImGui::DragFloat("Tilescale", &terrain.tilescale, 0.5f, 0.0f, 1000.0f);
    terrain_changed |= ImGui::InputInt("Octaves", &terrain.octaves);
    terrain_changed |= ImGui::SliderFloat("Lacunarity", &terrain.lacunarity, 0.0f, 30.0f);
    terrain_changed |= ImGui::SliderFloat("Persistance", &terrain.persistance, 0.0f, 20.0f);

    ImGui::End();

    return terrain_changed;
}

//Imma keep it 100 I legit just copy pasted the relevant parts from imgui_demo.cpp
void renderGUI::dockingDemo(ImGuiDemoDockspaceArgs* args, bool* p_open){
    ImGuiDockNodeFlags dockspace_flags = args->DockSpaceFlags;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    if (args->IsFullscreen)
    {
        // Fullscreen dockspace: practically the same as calling DockSpaceOverViewport();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;
    }
    else
    {
        // Floating dockspace
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!args->KeepWindowPadding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Window with a DockSpace", p_open, window_flags);
    if (!args->KeepWindowPadding)
        ImGui::PopStyleVar();

    if (args->IsFullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace widget inside our window
    // - Note that the id here is different from the one used by DockSpaceOverViewport(), so docking state won't get transfered between "Basic" and "Advanced" demos.
    // - If we made the ShowExampleAppDockSpaceBasic() calculate its own ID and pass it to DockSpaceOverViewport() the ID could easily match.
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}