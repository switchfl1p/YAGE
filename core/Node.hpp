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
    enum Type {PBR, EMISSIVE, PHONG, LAMBERTIAN} type = PBR;

    glm::vec4 phong_color;
    float shininess = 0.0f;

    glm::vec4 pbr_color;
    float metallic = 0.0f;
    float roughness = 0.5f;
};

struct Node{
    public:
        Material material;
        Transform transform;
};
