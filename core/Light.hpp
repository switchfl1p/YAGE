/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

struct AmbientLight{
    glm::vec4 intensity = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
};

struct PointLight{
    float attenuation = 1.0f;
    glm::vec4 intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 position;
};
