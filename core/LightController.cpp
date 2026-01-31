/* switchfl1p 2025-2026 */

#include <LightController.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>

LightController::LightController(PointLight &light)
    :point_light(light){}

void LightController::processPointLightInput(GLFWwindow* window, float delta_time){
    float movement_distance = movement_speed * delta_time;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        point_light.position.y += movement_distance;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        point_light.position.y -= movement_distance;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        radius += movement_distance;

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        radius -= movement_distance;
}

void LightController::rotatePointLight(float current_frame, float delta_time){
    static float paused_time = 0.0f;
    float curr_duration = fmod(current_frame - paused_time, loop_duration);

    if(rotate_flag){
        angle = (curr_duration / loop_duration) * 2.0f * glm::pi<float>();
        point_light.position.x = radius * cos(angle);
        point_light.position.z = radius * sin(angle);
        last_angle = angle;
    }
    else{
        paused_time += delta_time;
        point_light.position.x = radius * cos(last_angle);
        point_light.position.z = radius * sin(last_angle);
    }
}
