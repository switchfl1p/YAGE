/* switchfl1p 2025-2026 */

#pragma once
#include <Light.hpp>
#include <GLFW/glfw3.h>
#include <Timer.hpp>

class LightController{
    public:
        LightController(PointLight &light);

        void processPointLightInput(GLFWwindow* window, float delta_time);

        void rotatePointLight(Framework::Timer& timer);
        void halfRotatePointLight(Framework::Timer& timer);

        bool draw_flag = true;
        float movement_speed = 1.0f;
        float radius = 1.5f;

    private:
        PointLight &point_light;
        float angle = 0.0f;
};