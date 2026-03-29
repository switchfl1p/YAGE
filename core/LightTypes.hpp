#pragma once
#include <glm/glm.hpp>

glm::vec4 getValue(const std::pair<glm::vec4, float> &data);
float getTime(const std::pair<glm::vec4, float> &data);
float getValue(const std::pair<float, float> &data);
float getTime(const std::pair<float, float> &data);
float distance(const glm::vec3 &lhs, const glm::vec3 &rhs);