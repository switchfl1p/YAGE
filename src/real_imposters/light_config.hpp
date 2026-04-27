#pragma once

#include <LightManager.hpp>
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

    enum LightModels {
        LIGHT_CLASSIC,
        LIGHT_PBR
    };

    PointLightsData initPointLights();

    void setupLighValues(LightManager& light_manager, LightModels type);
}