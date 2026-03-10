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

void LightController::rotatePointLight(Framework::Timer& timer){
    angle = timer.GetAlpha() * 2.0f * glm::pi<float>();
    point_light.position.x = radius * cos(angle);
    point_light.position.z = radius * sin(angle);
}

void LightController::halfRotatePointLight(Framework::Timer& timer){
    float alpha = timer.GetAlpha();
    float bounce = (alpha < 0.5f) ? (alpha * 2.0f) : (2.0f - alpha * 2.0f);
    angle = bounce * glm::pi<float>();
    point_light.position.x = radius * cos(angle);
    point_light.position.z = radius * sin(angle);
}
