/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>

class Light{
    public:
        Light();
        ~Light();

        glm::vec4 light_direction = glm::vec4(0.866f, 0.5f, 0.0f, 0.0f);
        glm::vec4 light_intensity = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec4 ambient_intensity = glm::vec4( 0.1f, 0.1f, 0.1f, 1.0f);
};