#include <renderGUI.hpp>
#include "glm/gtc/type_ptr.hpp"

void renderGUI::renderNodeWindow(std::unordered_map<std::string, Node> &nodes){
    ImGui::Begin("Node Options");
    
    for (auto& [name, node] : nodes) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode(name.c_str())) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if(ImGui::TreeNode(("Material Properties##" + name).c_str())){
                std::string base_color_label = "base color##" + name;
                std::string metallic_label = "metallic##" + name;
                std::string roughness_label = "roughness##" + name;
                
                ImGui::ColorEdit4(base_color_label.c_str(), glm::value_ptr(node.material.base_color));
                ImGui::DragFloat(metallic_label.c_str(), &node.material.metallic, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat(roughness_label.c_str(), &node.material.roughness, 0.01f, 0.0f, 1.0f);
                
                ImGui::TreePop();
            }

            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
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
            ImGui::TreePop();  // Closes the node name tree (this was missing!)
        }
    }
    ImGui::End();
}

void renderGUI::renderLightWindow(Light &ambient_light, std::vector<PointLight> &point_lights){
    ImGui::Begin("Light Options");

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Ambient Light")) {
        ImGui::ColorEdit4("intensity##ambient", glm::value_ptr(ambient_light.intensity));
        ImGui::TreePop();
    }

    for(size_t i = 0; i < point_lights.size(); i++){

        ImGui::PushID(i);
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Point Light")) {
            ImGui::ColorEdit4("intensity", glm::value_ptr(point_lights[i].intensity));
            ImGui::DragFloat("attenuation", &point_lights[i].attenuation, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat3("position", glm::value_ptr(point_lights[i].position));
            ImGui::TreePop();
        }
        ImGui::PopID();  // Always pop after pushing!
    }
    ImGui::End();
}

void renderGUI::renderLightmodeOverlay(int light_model){
    ImGui::Begin("Lighting Model");

    const char* lighting_names[] = {
        "Lambertian",
        "Phong",
        "Blinn-Phong",
        "Gaussian",
        "PBR"
    };

    ImGui::Text("Current Model: %s", lighting_names[light_model]);
    ImGui::End();
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