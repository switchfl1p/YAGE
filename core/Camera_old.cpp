/* switchfl1p 2025-2026 */

#include <glm/ext/matrix_clip_space.hpp>
#include <Camera.hpp>

Camera::~Camera(){}

Camera::Builder& Camera::Builder::setCameraPos(glm::vec3 pos){
    camera_pos_b = pos;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraFront(glm::vec3 front){
    camera_front_b = front;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraUp(glm::vec3 up){
    camera_up_b = up;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraYaw(float yaw){
    camera_yaw_b = yaw;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraPitch(float pitch){
    camera_pitch_b = pitch;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraSpeed(float speed){
    camera_speed_b = speed;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraZoomSensibility(float z_sens){
    zoom_sens_b = z_sens;
    return *this;
}

Camera::Builder& Camera::Builder::setCameraMouseSensibility(float m_sens){
    mouse_sens_b = m_sens;
    return *this;
}

Camera::Builder& Camera::Builder::setPerspectiveFoV(float desired_fov){
    fov_b = desired_fov;
    return *this;
}

Camera::Builder& Camera::Builder::setPerspZNear(float p_zN){
    persp_zNear_b = p_zN;
    return *this;
}

Camera::Builder& Camera::Builder::setPerspZFar(float p_zF){
    persp_zFar_b = p_zF;
    return *this;
}

void Camera::updatePerspMat(){
    perspective_mat = glm::perspective(glm::radians(fov), float(window_width)/float(window_height), camera_pers_zNear, camera_pers_zFar);
}

void Camera::updateDirection(){
    direction.x = cos(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
    direction.y = sin(glm::radians(camera_pitch));
    direction.z = sin(glm::radians(camera_yaw)) * cos(glm::radians(camera_pitch));
    camera_front = glm::normalize(direction);
}

void Camera::updateViewMat(){
    view_mat = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
}

glm::mat4 Camera::getPerspMat(){
    return perspective_mat;
}

glm::mat4 Camera::getViewMat(){
    return view_mat;
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch,
    float speed, float z_sens, float m_sens, float f, int w_width, int w_height, float p_zNear, float p_zFar)
:   camera_pos(pos),
    camera_front(front),
    camera_up(up),
    camera_yaw(yaw),
    camera_pitch(pitch),
    camera_speed(speed),
    zoom_sens(z_sens),
    mouse_sens(m_sens),
    fov(f),
    first_mouse(true),
    camera_pers_zNear(p_zNear),
    camera_pers_zFar(p_zFar),
    window_width(w_width),
    window_height(w_height)
{
    updatePerspMat();
    updateDirection();
    updateViewMat();    
}


Camera Camera::Builder::build(){
    return Camera(camera_pos_b, camera_front_b, camera_up_b, camera_yaw_b, camera_pitch_b, camera_speed_b,
                 zoom_sens_b, mouse_sens_b, fov_b, width_b, height_b, persp_zNear_b, persp_zFar_b);
}