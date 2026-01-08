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
    int w = 500;
    int h = 400;

    Camera camera1 = Camera::Builder().build();

    std::cout << "Testing Camera with default values:\n";
    std::cout << "View Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera1.getViewMat());

    std::cout << "\n";
    std::cout << "Perspective Matrix:\n";
    std::cout << "\n";
    printMat4Pretty(camera1.getPerspMat());
}