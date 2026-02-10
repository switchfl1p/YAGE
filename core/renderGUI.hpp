#pragma once
#include <imgui.h>
#include <Node.hpp>
#include <string>
#include <Light.hpp>

namespace renderGUI{
    void renderNodeWindow(std::unordered_map<std::string, Node> &nodes, AmbientLight ambient_light, PointLight point_light);
    void renderLightmodeOverlay(int light_model);
}