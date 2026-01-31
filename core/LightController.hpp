/* switchfl1p 2025-2026 */

#pragma once
#include <Light.hpp>
#include <GLFW/glfw3.h>

class LightController{
    public:
        LightController(PointLight &light);

        void processPointLightInput(GLFWwindow* window, float delta_time);
        void rotatePointLight(float current_frame, float delta_time);
        void halfRotatePointLight(float current_frame, float delta_time);

        bool draw_flag = true;
        bool rotate_flag = true;
        float movement_speed = 1.0f;
        float radius = 1.5f;
        float loop_duration = 10.0f;

    private:
        PointLight &point_light;
        float last_angle = 0.0f;
        float angle = 0.0f;
};