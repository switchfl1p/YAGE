/* switchfl1p 2025-2026 */

#include "Camera.hpp"
#include <iostream>
#include <iomanip>

void printMat4Pretty(const glm::mat4& mat) {
    std::cout << std::fixed << std::setprecision(3);
    for (int i = 0; i < 4; i++) {
        std::cout << "[ ";
        for (int j = 0; j < 4; j++) {
            std::cout << std::setw(8) << mat[j][i];
            if (j < 3) std::cout << ", ";
        }
        std::cout << " ]\n";
    }
}

int main(){
    Camera camera1;

    std::cout << "Testing Camera with default values:\n";
    std::cout << "View Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera1.getViewMat());

    std::cout << "\n";
    std::cout << "Perspective Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera1.getPerspMat());
    std::cout << "\n";

    glm::vec3 camera_pos_b = glm::vec3(5.0f, 5.5f, 5.0f);
    glm::vec3 camera_front_b = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 camera_up_b = glm::vec3(0.0f, 1.0f, 0.0f);

    float camera_yaw_b = -45.0f;
    float camera_pitch_b = 5.0f;
    float fov_b = 25.0f;
    float persp_zNear_b = 1.0f;
    float persp_zFar_b = 50.0f;

    int width = 25;
    int height = 25;

    Camera camera2;

    camera2.position = camera_pos_b;
    camera2.front = camera_front_b;
    camera2.up = camera_up_b;

    camera2.yaw = camera_yaw_b;
    camera2.pitch = camera_pitch_b;
    camera2.fov = fov_b;
    camera2.persp_zNear = persp_zNear_b;
    camera2.persp_zFar = persp_zFar_b;
    camera2.viewport_w = width;
    camera2.viewport_h = height;

    camera2.updateCamera();
    
    std::cout << "Testing Camera with custom values:\n";
    std::cout << "View Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera2.getViewMat());

    std::cout << "\n";
    std::cout << "Perspective Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera2.getPerspMat());
    std::cout << "\n";
    
}