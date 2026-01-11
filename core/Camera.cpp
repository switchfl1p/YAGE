/* switchfl1p 2025-2026 */

#include <Camera.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

Camera::Camera(int viewport_width, int viewport_height)
    :viewport_w(viewport_width),
    viewport_h(viewport_height)
{
    if (viewport_h == 0) {
        std::cerr << "Warning: viewport height is 0!" << std::endl;
        viewport_h = 1;  // Prevent division by zero
    }
    updateCamera();
}

Camera::~Camera(){}

void Camera::updateCamera(){
    updateViewMat();
    updatePerspMat();
}

glm::mat4 Camera::getViewMat() const{
    return view_mat;
}

glm::mat4 Camera::getPerspMat() const{
    return perspective_mat;
}

void Camera::updateDirection(){
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::updateViewMat(){
    updateDirection();
    view_mat = glm::lookAt(position, position + front, up);
}

void Camera::updatePerspMat(){
    perspective_mat = glm::perspective(glm::radians(fov), float(viewport_w)/float(viewport_h), persp_zNear, persp_zFar);
}