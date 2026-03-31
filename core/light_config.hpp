#pragma once

#include <Timer.hpp>
#include <Interpolators.hpp>
#include <InterpolatorTraits.hpp>
#include <array>
#include <glm/glm.hpp>

namespace light_config {
    constexpr int NUMBER_OF_POINT_LIGHTS = 3;
    constexpr int NUMBER_OF_DIR_LIGHTS = 2;

    struct PointLightsData {
        std::array<ConstVelLinearInterpolator<glm::vec3>, NUMBER_OF_POINT_LIGHTS> pl_interpolators;
        std::array<Timer, NUMBER_OF_POINT_LIGHTS> pl_timers;
    };

    //std::array<Timer, NUMBER_OF_DIR_LIGHTS> dir_timers;
    //std::array<glm::vec4, NUMBER_OF_POINT_LIGHTS> pl_intensities;

    PointLightsData initPointLights();
}