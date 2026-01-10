/* switchfl1p 2025-2026 */

#include <Camera.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

Camera::Camera(){
    updateCamera();
}

Camera::~Camera(){}

void Camera::updateCamera(){
    updateDirection();
    updateViewMat();
    updatePerspMat();
}

glm::mat4 Camera::getViewMat(){
    return view_mat;
}

glm::mat4 Camera::getPerspMat(){
    return perspective_mat;
}

void Camera::updateDirection(){
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::updateViewMat(){
    view_mat = glm::lookAt(position, position + front, up);
}

void Camera::updatePerspMat(){
    perspective_mat = glm::perspective(glm::radians(fov), float(window_width)/float(window_height), persp_zNear, persp_zFar);
}