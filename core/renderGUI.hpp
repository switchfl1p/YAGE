#pragma once
#include <imgui.h>
#include <Node.hpp>
#include <string>
#include <Light.hpp>

namespace renderGUI{
    void renderNodeWindow(std::unordered_map<std::string, Node> &nodes);
    void renderLightWindow(Light &ambient_light, std::vector<PointLight> &point_lights);
    void renderLightmodeOverlay(int light_model);

    struct ImGuiDemoDockspaceArgs
    {
        bool                IsFullscreen = true;
        bool                KeepWindowPadding = false; // Keep WindowPadding to help understand that DockSpace() is a widget inside the window.
        ImGuiDockNodeFlags  DockSpaceFlags  = ImGuiDockNodeFlags_None;
    };

    void dockingDemo(ImGuiDemoDockspaceArgs* args, bool* p_open);
}