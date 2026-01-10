/* switchfl1p 2025-2026 */

#include <CameraController.hpp>

CameraController::CameraController(Camera &cam)
    :camera(cam){}

CameraController::~CameraController(){}

void CameraController::processCameraInput(GLFWwindow* window, float delta_time){
    updateCameraSpeed(delta_time);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += movement_speed * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= movement_speed * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * movement_speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * movement_speed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += movement_speed * camera.up;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= movement_speed * camera.up;
}