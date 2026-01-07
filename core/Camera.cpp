/* switchfl1p 2025-2026 */

#include <Camera.hpp>

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

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch, float speed, float z_sens, float m_sens, float f)
:   camera_pos(pos),
    camera_front(front),
    camera_up(up),
    camera_yaw(yaw),
    camera_pitch(pitch),
    camera_speed(speed),
    zoom_sens(z_sens),
    mouse_sens(m_sens),
    fov(f),
    first_mouse(true)
{
    //view_mat
    //persp_mat
    //direction
    //last_x
    //last_y
    
}