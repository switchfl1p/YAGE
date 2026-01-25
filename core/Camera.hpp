/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

class Camera{
    public:
        Camera(int viewport_width, int viewport_height);

        glm::mat4 getPerspMat() const;
        glm::mat4 getViewMat() const;

        void updateCamera();
        void updateViewMat();
        void updatePerspMat();
        void updateDirection();

        glm::vec3 position = glm::vec3(0.0f, 1.5f, 10.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        float yaw = -90.0f;
        float pitch = 0.0f;

        //perspective related
        int viewport_w = 1280;
        int viewport_h = 720;
        float fov = 45.0f;
        float persp_zNear = 0.1f;
        float persp_zFar = 100.0f;

    private:
        glm::vec3 direction;

        glm::mat4 view_mat;
        glm::mat4 perspective_mat;
};