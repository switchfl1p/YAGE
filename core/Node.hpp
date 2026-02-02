/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

class Transform{
    public:
        glm::mat4 model_mat = glm::mat4(1);
        glm::mat4 calc_model_mat();

        glm::vec3 translation_component = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 rotation_component = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 scale_component = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct Material{
    glm::vec4 base_color;
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    // Non-PBR shaders
    glm::vec4 diffuse_color;
    float shininess_factor;
};

struct Node{
    public:
        Material material;
        Transform transform;
};
