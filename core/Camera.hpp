/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

class Camera{
    public:
        Camera();
        ~Camera();

        glm::mat4 getPerspMat();
        glm::mat4 getViewMat();

        void updateCamera();

        glm::vec3 position = glm::vec3(0.0f, 1.5f, 10.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        float yaw = -90.0f;
        float pitch = 0.0f;

        //perspective related
        float fov = 45.0f;
        float persp_zNear = 0.1f;
        float persp_zFar = 100.0f;

        //controller related
        float movement_speed = 10.0f;
        float mouse_sens = 0.025f;
        float zoom_sens = 2.0; //not sure about this one

        //not correct
        int window_width = 500;
        int window_height = 400;

    private:
        glm::vec3 direction;

        glm::mat4 view_mat;
        glm::mat4 perspective_mat;

        bool first_mouse = true;

        void updateDirection();
        void updateViewMat();
        void updatePerspMat();
};