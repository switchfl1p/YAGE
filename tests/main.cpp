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

    Camera camera1 = Camera::Builder().build();

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

    Camera camera2 = Camera::Builder()
        .setCameraPos(camera_pos_b)
        .setCameraFront(camera_front_b)
        .setCameraUp(camera_up_b)
        .setCameraYaw(camera_yaw_b)
        .setCameraPitch(camera_pitch_b)
        .setPerspectiveFoV(fov_b)
        .setPerspZNear(persp_zNear_b)
        .setPerspZFar(persp_zFar_b)
        .build();
    
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