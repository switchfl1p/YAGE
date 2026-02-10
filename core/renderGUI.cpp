#include <renderGUI.hpp>
#include "glm/gtc/type_ptr.hpp"

void renderGUI::renderNodeWindow(std::unordered_map<std::string, Node> &nodes, AmbientLight ambient_light, PointLight point_light){
    ImGui::Begin("Node Options");
    ImGui::Text("Centerpiece Material");
    ImGui::ColorEdit4("base color##sphere", glm::value_ptr(nodes["sphere"].material.base_color));
    ImGui::DragFloat("metallic##sphere", &nodes["sphere"].material.metallic, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("roughness##sphere", &nodes["sphere"].material.roughness, 0.01f, 0.0f, 1.0f);

    ImGui::Text("Plane Material");
    ImGui::ColorEdit4("base color##plane", glm::value_ptr(nodes["plane"].material.base_color));
    ImGui::DragFloat("metallic##plane", &nodes["plane"].material.metallic, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("roughness##plane", &nodes["plane"].material.roughness, 0.01f, 0.0f, 1.0f);

    ImGui::Text("Light Options");
    ImGui::ColorEdit4("ambient intensity", glm::value_ptr(ambient_light.intensity));

    ImGui::ColorEdit4("light intensity", glm::value_ptr(point_light.intensity));
    ImGui::DragFloat("light attenuation", &point_light.attenuation, 0.01f, 0.0f, 5.0f);
    ImGui::End();
}

void renderGUI::renderLightmodeOverlay(int light_model){
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Lighting Model", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    const char* lighting_names[] = {
        "Lambertian",
        "Phong",
        "Blinn-Phong",
        "Gaussian",
        "PBR"
    };

    ImGui::Text("Current Model: %s", lighting_names[light_model]);
}