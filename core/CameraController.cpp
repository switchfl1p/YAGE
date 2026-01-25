/* switchfl1p 2025-2026 */

#include <CameraController.hpp>

CameraController::CameraController(Camera &cam)
    :camera(cam){}

void CameraController::processCameraInput(GLFWwindow* window, float delta_time){
    updateCameraSpeed(delta_time);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += movement_distance * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= movement_distance * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * movement_distance;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * movement_distance;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += movement_distance * camera.up;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= movement_distance * camera.up;
}

void CameraController::mouseCameraController(GLFWwindow* window, double x_pos, double y_pos){
    if(first_mouse){
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    x_offset *= mouse_sens;
    y_offset *= mouse_sens;

    camera.yaw += x_offset;
    camera.pitch += y_offset;

    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    camera.updateDirection();
}

void CameraController::mouseZoomController(GLFWwindow* window, double x_offset, double y_offset){
    camera.fov -= (float)y_offset * zoom_sens;

    if (camera.fov < 1.0f)
        camera.fov = 1.0f;
    if (camera.fov > 45.0f)
        camera.fov = 45.0f; 
}

void CameraController::updateCameraSpeed(float delta_time){
    movement_distance = movement_speed * delta_time;
}