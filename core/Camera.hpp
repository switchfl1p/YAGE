/* switchfl1p 2025-2026 */

#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera{
    public:
        ~Camera();

        void processKeyboardInput(GLFWwindow* window, float delta_time);
        void mouseCameraCallback(GLFWwindow* window, double x_pos, double y_pos);
        void mouseZoomCallback(GLFWwindow* window, double x_offset, double y_offset);

        void updateDirection();
        void updateView();
        glm::mat4 getPerspMat();

        class Builder{
            public:
                Builder& setCameraPos(glm::vec3 pos);
                Builder& setCameraFront(glm::vec3 front);
                Builder& setCameraUp(glm::vec3 up);
                Builder& setCameraYaw(float yaw);
                Builder& setCameraPitch(float pitch);
                Builder& setCameraSpeed(float speed);
                Builder& setCameraZoomSensibility(float z_sens);
                Builder& setCameraMouseSensibility(float m_sens);
                Builder& setPerspectiveFoV(float desired_fov);

                Camera build();
            private:
                glm::vec3 camera_pos_b = glm::vec3(0.0f, 1.5f, 10.0f);
                glm::vec3 camera_front_b = glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 camera_up_b = glm::vec3(0.0f, 1.0f, 0.0f);

                float camera_yaw_b = -90.0f;
                float camera_pitch_b = 0.0f;
                float camera_speed_b = 10.0f;
                float zoom_sens_b = 2.0;
                float mouse_sens_b = 0.025f;
                float fov_b = 45.0f;
        };

    private:
        Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch, float speed, float z_sens, float m_sens, float fov);

        glm::vec3 camera_pos;
        glm::vec3 camera_front;
        glm::vec3 camera_up;

        glm::mat4 view_mat;
        glm::mat4 perspective_mat;

        float camera_speed;

        void updateCameraSpeed(float &delta_time);

        float camera_yaw;
        float camera_pitch;
        glm::vec3 direction;

        float last_x;
        float last_y;
        bool first_mouse;
        float zoom_sens;
        float mouse_sens;

        float fov;
};