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

struct DirLightValue{
    float norm_time;
    glm::vec4 ambient;
	glm::vec4 dl_intensity;
	glm::vec4 background_color;
    float max_intensity;
};

enum TimerTypes
{
	TIMER_SUN,
	TIMER_LIGHTS,
	TIMER_ALL,
	NUM_TIMER_TYPES,
};

//check which stuff is actually being used
class LightManager{
    public:
        LightManager(std::span<ConstVelLinearInterpolator<glm::vec3>> interpolators, std::span<Timer> timers);

        void setDirLightValues(std::span<DirLightValue> dir_light_values);
        glm::vec4 getDirLightDirection() const;
        glm::vec4 getDirLightIntensity() const;
        int getNumberOfDirLights() const;

        void setPointLightIntensity(int light_index, const glm::vec4 &intensity);
        glm::vec4 getPointLightIntensity(int light_index) const;
        glm::vec3 getWorldLightPosition(int light_index) const; //dl or pl? does it matter? All experts agree on pl, dls dont have positions. Needs testing tho
        int getNumberOfPointLights() const;
        float GetDirLightTime() const;

        LightBlock getLightInformation(const glm::mat4 &world_to_camera_mat) const; //what type of light?
        glm::vec4 getBackgroundColor() const;
        float getMaxIntensity() const;

        void updateTime();
        void togglePause(TimerTypes timer);
        void setPause(TimerTypes timer, bool pause = true);
        bool isPaused(TimerTypes timer) const;
        void rewindTime(TimerTypes timer, float sec_rewind);
        void fastForwardTime(TimerTypes timer, float sec_rewind);
        void CreateTimer(const std::string &timer_name, Timer::Type type, float duration);
	    float GetTimerValue(const std::string &timer_name) const;
    
    private:
        std::vector<TimedLinearInterpolator<glm::vec4>> dl_interpolators;
        std::vector<glm::vec4> dl_intensities; //is this needed?
        std::vector<Timer> dl_timers;

        std::vector<ConstVelLinearInterpolator<glm::vec3>> pl_interpolators;
        std::vector<glm::vec4> pl_intensities;
        std::vector<Timer> pl_timers;

        std::vector<TimedLinearInterpolator<glm::vec4>> ambient_interpolators;
	    std::vector<TimedLinearInterpolator<glm::vec4>> background_interpolators;
	    std::vector<TimedLinearInterpolator<float>> max_intensity_interpolators;

        std::map<std::string, Timer> extra_timers;
};
