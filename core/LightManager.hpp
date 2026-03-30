/* switchfl1p 2025-2026 */

#pragma once
#include <glm/glm.hpp>
#include <Light.hpp>
#include <Timer.hpp>
#include <Interpolators.hpp>
#include <string>
#include <map>
#include <span>

enum LightingModel
{
    LM_LAMBERTIAN = 0,
    LM_PHONG_LIGHTING,
    LM_BLINN_LIGHTING,
    LM_GAUSSIAN_LIGHTING,
    LM_PBR_LIGHTING,

    LM_COUNT,
};

constexpr int NUMBER_OF_POINT_LIGHTS = 3;
constexpr int NUMBER_OF_DIR_LIGHTS = 2;

struct LightBlock{
    PointLight point_lights[NUMBER_OF_POINT_LIGHTS];
    DirectionalLight dir_lights[NUMBER_OF_DIR_LIGHTS];
    AmbientLight ambient_light;
    int point_light_count;
    int dir_light_count;
    float max_intensity;
    float gamma;
};

struct SunlightValue{
    float norm_time;
    glm::vec4 ambient;
	glm::vec4 sunlightIntensity;
	glm::vec4 backgroundColor;
    float max_intensity;
};

enum TimerTypes
{
	TIMER_SUN,
	TIMER_LIGHTS,
	TIMER_ALL,
	NUM_TIMER_TYPES,
};

class LightManager{
    public:
        LightManager();
        void setSunlightValues(std::span<SunlightValue> sun_values);

        void updateTime();
        void togglePause(TimerTypes timer);
        void setPause(TimerTypes timer, bool pause = true);
        bool isPaused(TimerTypes timer) const;

        void rewindTime(TimerTypes timer, float sec_rewind);
        void fastForwardTime(TimerTypes timer, float sec_rewind);

        LightBlock getLightInformation(const glm::mat4 &world_to_camera_mat) const;
        glm::vec4 getBackgroundColor() const;
        float getMaxIntensity() const;

        glm::vec4 getSunlightDirection() const;
	    glm::vec4 getSunlightIntensity() const;

        int getNumberOfPointLights() const;
        glm::vec3 getWorldLightPosition(int light_index) const;
        void setPointLightIntensity(int light_index, const glm::vec4 &intensity);
        glm::vec4 getPointLightIntensity(int light_index) const;

        void CreateTimer(const std::string &timer_name, Timer::Type type, float duration);
	    float GetTimerValue(const std::string &timer_name) const;
	    float GetSunTime() const;
    
    private:
        Timer sun_timer;
        TimedLinearInterpolator<glm::vec4> ambient_interpolator;
	    TimedLinearInterpolator<glm::vec4> background_interpolator;
	    TimedLinearInterpolator<glm::vec4> sunlight_interpolator;
	    TimedLinearInterpolator<float> max_intensity_interpolator;
        std::vector<ConstVelLinearInterpolator<glm::vec3>> light_pos_interpolators;
        
        std::vector<glm::vec4> light_intensities;
        std::vector<Timer> light_timers;
        std::map<std::string, Timer> extra_timers;
};
