/* switchfl1p 2025-2026 */

#pragma once
#include <imgui.h>
#include <Node.hpp>
#include <string>
#include <Light.hpp>
#include <Terrain.hpp>
#include <span>

namespace imgui_util{
    void renderNodeWindow(std::unordered_map<std::string, Node> &nodes, int light_model);
    void renderLightWindow(AmbientLight &ambient_light, std::span<PointLight> point_lights, std::span<DirectionalLight> dir_lights);
    void renderStatusOverlay(int light_model, bool sun_movement_flag, bool point_light_movement_flag, bool camera_status);
    bool renderTerrainWindow(TerrainData &terrain);

    struct ImGuiDemoDockspaceArgs
    {
        bool                IsFullscreen = true;
        bool                KeepWindowPadding = false; // Keep WindowPadding to help understand that DockSpace() is a widget inside the window.
        ImGuiDockNodeFlags  DockSpaceFlags  = ImGuiDockNodeFlags_None;
    };

    void dockingDemo(ImGuiDemoDockspaceArgs* args, bool* p_open);
}