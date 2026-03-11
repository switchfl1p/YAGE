/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>
#include <Light.hpp>

enum LightingModel
{
    LM_LAMBERTIAN = 0,
    LM_PHONG_LIGHTING,
    LM_BLINN_LIGHTING,
    LM_GAUSSIAN_LIGHTING,
    LM_PBR_LIGHTING,

    LM_COUNT,
};

constexpr int MAX_POINT_LIGHTS = 2;
constexpr int MAX_DIR_LIGHTS = 1;

struct LightBlock{
    PointLight point_lights[MAX_POINT_LIGHTS];
    DirectionalLight dir_lights[MAX_DIR_LIGHTS];
    AmbientLight ambient_light;
    int point_light_count;
    int dir_light_count;
};

struct SunlightValue{
    float normTime;
    glm::vec4 ambient;
	glm::vec4 sunlightIntensity;
	glm::vec4 backgroundColor;
};

class LightManager{
    public:
        void setSunlightValues();
};
