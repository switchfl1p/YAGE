/* switchfl1p 2025-2026 */

#pragma once

#include <Camera.hpp>
#include <GLFW/glfw3.h>

/* Currently min fov 1.0, max fov 45.0 
min pitch -89.0f, max pitch 89.0 */

class CameraController{
    public:
        CameraController(Camera &cam);
        ~CameraController();

        float movement_speed = 10.0f;
        float mouse_sens = 0.025f;
        float zoom_sens = 2.0f;

        float last_x = 0;
        float last_y = 0;

        bool first_mouse = true;
        
        void processCameraInput(GLFWwindow* window, float delta_time);

        void mouseCameraController(GLFWwindow* window, double x_pos, double y_pos);
        void mouseZoomController(GLFWwindow* window, double x_offset, double y_offset);

    private:
        Camera &camera;
        void updateCameraSpeed(float delta_time);
        float movement_distance;
};
