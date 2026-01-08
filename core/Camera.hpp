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
        void updateViewMat();
        void updatePerspMat();

        glm::mat4 getPerspMat();
        glm::mat4 getViewMat();

        class Builder{
            public:
                Builder(int* w, int* h);
                Builder& setCameraPos(glm::vec3 pos);
                Builder& setCameraFront(glm::vec3 front);
                Builder& setCameraUp(glm::vec3 up);
                Builder& setCameraYaw(float yaw);
                Builder& setCameraPitch(float pitch);
                Builder& setCameraSpeed(float speed);
                Builder& setCameraZoomSensibility(float z_sens);
                Builder& setCameraMouseSensibility(float m_sens);
                Builder& setPerspectiveFoV(float desired_fov);
                Builder& setPerspZNear(float p_zN);
                Builder& setPerspZFar(float p_zF);

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
                float persp_zNear_b = 0.1f;
                float persp_zFar_b = 100.0f;

                int* width_b;
                int* height_b;
        };

    private:
        Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch, float speed, 
            float z_sens, float m_sens, float fov, int* w_width, int* w_height, float p_zNear, float p_zFar);
        
        int* window_width;
        int* window_height;

        glm::vec3 camera_pos;
        glm::vec3 camera_front;
        glm::vec3 camera_up;
        glm::vec3 direction;

        glm::mat4 view_mat;
        glm::mat4 perspective_mat;

        float camera_speed;
        float camera_yaw;
        float camera_pitch;
        float zoom_sens;
        float mouse_sens;
        float camera_pers_zNear;
        float camera_pers_zFar;
        float fov;

        bool first_mouse;

        void updateCameraSpeed(float &delta_time);
};