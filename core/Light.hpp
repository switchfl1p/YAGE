/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

struct AmbientLight{
    glm::vec4 intensity = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
};

struct PointLight{
    glm::vec4 intensity = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 position;
    float attenuation = 1.0f;
    float padding[3]; //for UBO usage, size needs to multiple of 16
};

struct DirectionalLight{
    glm::vec4 intensity = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 direction;
};