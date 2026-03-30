/* switchfl1p 2025-2026 */

#pragma once

#include <Interpolators.hpp>
#include <glm/glm.hpp>

//For a Timed Linear Interpolator using glm::vec4
template<>
struct InterpolatorTraits<std::pair<glm::vec4, float>> {
    static glm::vec4 getValue(const std::pair<glm::vec4, float> &data) {
        return data.first;
    }
    static float getTime(const std::pair<glm::vec4, float> &data) {
        return data.second;
    }
};

//For a Constant Velocity Linear Interpolator using glm::vec3
template<>
struct InterpolatorTraits<glm::vec3> {
    static float distance(const glm::vec3 &a, const glm::vec3 &b) {
        return glm::length(b - a);
    }
};

//For a Timed Linear Interpolator using float
template<>
struct InterpolatorTraits<std::pair<float, float>> {
    static float getValue(const std::pair<float, float> &data) {
        return data.first;
    }
    static float getTime(const std::pair<float, float> &data) {
        return data.second;
    }
};