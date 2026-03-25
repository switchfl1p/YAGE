/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

struct Transform{
    glm::mat4 model_mat = glm::mat4(1);
    glm::mat4 calc_model_mat();

    glm::vec3 translation_component = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 rotation_component = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 scale_component = glm::vec3(1.0f, 1.0f, 1.0f);
};

//for Lambertian, Phong, Blinn-Phong and Gaussian
struct ClassicProperties{
    glm::vec4 color;
    float shininess;
};

struct PBRProperties{
    glm::vec4 color;
    float metallic;
    float roughness;
};

struct Material{
    ClassicProperties classic;
    PBRProperties pbr;
    bool is_emissive = false;
};

struct Node{
    Material material;
    Transform transform;
};

struct MaterialBlock{
    glm::vec4 classic_color;
    glm::vec4 pbr_color;
    float shininess_factor;
    float metallic;
    float roughness;
    float is_emissive;

    void getMaterialInformation(const Node& node);
};