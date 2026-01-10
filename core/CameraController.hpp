/* switchfl1p 2025-2026 */

#pragma once

#include <Camera.hpp>
#include <GLFW/glfw3.h>

class CameraController{
    public:
        CameraController(Camera &cam);
        ~CameraController();

        float movement_speed = 10.0f;
        float mouse_sens = 0.025f;
        float zoom_sens = 2.0f;

        bool first_mouse = true;
        
        void processCameraInput(GLFWwindow* window, float delta_time);

        void mouseCameraController(GLFWwindow* window, double x_pos, double y_pos);
        void mouseZoomController(GLFWwindow* window, double x_offset, double y_offset);

    private:
        Camera &camera;
        void updateCameraSpeed(float delta_time);
};
