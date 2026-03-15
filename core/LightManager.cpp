#include "glm/ext/matrix_transform.hpp"
#include <LightManager.hpp>

LightManager::LightManager()
    : sun_timer(Framework::Timer::TT_LOOP, 30.0f)
    , ambient_interpolator()
{
    light_timers.reserve(NUMBER_OF_POINT_LIGHTS);

    light_pos_interpolators.resize(NUMBER_OF_POINT_LIGHTS, Framework::ConstVelLinearInterpolator<glm::vec3>());
    light_intensities.resize(NUMBER_OF_POINT_LIGHTS, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    std::vector<glm::vec3> pos_values;
    pos_values.reserve(60);

    pos_values.push_back(glm::vec3(-50.0f, 30.0f, 70.0f));
	pos_values.push_back(glm::vec3(-70.0f, 30.0f, 50.0f));
	pos_values.push_back(glm::vec3(-70.0f, 30.0f, -50.0f));
	pos_values.push_back(glm::vec3(-50.0f, 30.0f, -70.0f));
	pos_values.push_back(glm::vec3(50.0f, 30.0f, -70.0f));
	pos_values.push_back(glm::vec3(70.0f, 30.0f, -50.0f));
	pos_values.push_back(glm::vec3(70.0f, 30.0f, 50.0f));
	pos_values.push_back(glm::vec3(50.0f, 30.0f, 70.0f));
	light_pos_interpolators[0].SetValues(pos_values);
	light_timers.push_back(Framework::Timer(Framework::Timer::TT_LOOP, 15.0f));

	//Right-side light.
	pos_values.clear();
	pos_values.push_back(glm::vec3(100.0f, 6.0f, 75.0f));
	pos_values.push_back(glm::vec3(90.0f, 8.0f, 90.0f));
	pos_values.push_back(glm::vec3(75.0f, 10.0f, 100.0f));
	pos_values.push_back(glm::vec3(60.0f, 12.0f, 90.0f));
	pos_values.push_back(glm::vec3(50.0f, 14.0f, 75.0f));
	pos_values.push_back(glm::vec3(60.0f, 16.0f, 60.0f));
	pos_values.push_back(glm::vec3(75.0f, 18.0f, 50.0f));
	pos_values.push_back(glm::vec3(90.0f, 20.0f, 60.0f));
	pos_values.push_back(glm::vec3(100.0f, 22.0f, 75.0f));
	pos_values.push_back(glm::vec3(90.0f, 24.0f, 90.0f));
	pos_values.push_back(glm::vec3(75.0f, 26.0f, 100.0f));
	pos_values.push_back(glm::vec3(60.0f, 28.0f, 90.0f));
	pos_values.push_back(glm::vec3(50.0f, 30.0f, 75.0f));

	pos_values.push_back(glm::vec3(105.0f, 9.0f, -70.0f));
	pos_values.push_back(glm::vec3(105.0f, 10.0f, -90.0f));
	pos_values.push_back(glm::vec3(72.0f, 20.0f, -90.0f));
	pos_values.push_back(glm::vec3(72.0f, 22.0f, -70.0f));
	pos_values.push_back(glm::vec3(105.0f, 32.0f, -70.0f));
	pos_values.push_back(glm::vec3(105.0f, 34.0f, -90.0f));
	pos_values.push_back(glm::vec3(72.0f, 44.0f, -90.0f));

	light_pos_interpolators[1].SetValues(pos_values);
	light_timers.push_back(Framework::Timer(Framework::Timer::TT_LOOP, 25.0f));
}

glm::vec4 GetValue(const LightVectorData &data) {return data.first;}
float GetTime(const LightVectorData &data) {return data.second;}
float GetValue(const MaxIntensityData &data) {return data.first;}
float GetTime(const MaxIntensityData &data) {return data.second;}
float distance(const glm::vec3 &lhs, const glm::vec3 &rhs)
{
	return glm::length(rhs - lhs);
}

void LightManager::setSunlightValues(std::span<SunlightValue> sun_values){
	std::vector<std::pair<glm::vec4, float>> ambient;
	std::vector<std::pair<glm::vec4, float>> light;
	std::vector<std::pair<glm::vec4, float>> background;

	for(auto& value : sun_values){
		ambient.push_back(std::pair<glm::vec4, float>(value.ambient, value.norm_time));
		light.push_back(std::pair<glm::vec4, float>(value.sunlightIntensity, value.norm_time));
		background.push_back(std::pair<glm::vec4, float>(value.backgroundColor, value.norm_time));
	}

	ambient_interpolator.SetValues(ambient);
	sunlight_interpolator.SetValues(light);
	background_interpolator.SetValues(background);

	std::vector<std::pair<float, float>> max_intensity; 
	max_intensity.push_back(std::pair<float, float>(1.0f, 0.0f));
	max_intensity_interpolator.SetValues(max_intensity, false);
}

void LightManager::updateTime(){
	sun_timer.Update();

	for(auto& timer : light_timers){
		timer.Update();
	}
	for(auto& [name, timer] : extra_timers){
		timer.Update();
	}
}

void LightManager::setPause(TimerTypes timer, bool pause){
	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.SetPause(pause);
		}
		
		for(auto& [name, timer] : extra_timers){
			timer.SetPause(pause);
		}
	}

	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.TogglePause();
	}
}

void LightManager::togglePause(TimerTypes timer){
	setPause(timer, !isPaused(timer));
}

bool LightManager::isPaused(TimerTypes timer) const{
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		return sun_timer.IsPaused();
	}
	return light_timers.front().IsPaused();
}

void LightManager::rewindTime(TimerTypes timer, float sec_rewind){
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.Rewind(sec_rewind);
	}

	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.Rewind(sec_rewind);
		}

		for(auto& [name, timer] : extra_timers){
			timer.Rewind(sec_rewind);
		}
	}
}

void LightManager::fastForwardTime(TimerTypes timer, float secFF){
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.Fastforward(secFF);
	}

	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.Fastforward(secFF);
		}

		for(auto& [name, timer] : extra_timers){
			timer.Fastforward(secFF);
		}
	}
}

LightBlock LightManager::getLightInformation(const glm::mat4 &world_to_camera_mat) const{
	LightBlock light_data;

	light_data.ambient_light.intensity = ambient_interpolator.Interpolate(sun_timer.GetAlpha());
	//attenuation

	light_data.dir_lights[0].direction = world_to_camera_mat * getSunlightDirection();
	light_data.dir_lights[0].intensity = sunlight_interpolator.Interpolate(sun_timer.GetAlpha());

	for(int i = 0; i < light_data.point_light_count; i++){
		glm::vec4 world_light_pos = glm::vec4(light_pos_interpolators[i].Interpolate(light_timers[i].GetAlpha()), 1.0f);
		glm::vec4 light_pos_cam_space = world_to_camera_mat * world_light_pos;

		light_data.point_lights[i].position = light_pos_cam_space;
		light_data.point_lights[i].intensity = light_intensities[i];
	}
	return light_data;
}

//rotates sunlight and returns it
glm::vec4 LightManager::getSunlightDirection() const{
	float angle = 2.0f * 3.14159f * sun_timer.GetAlpha();
	glm::vec4 sun_direction(0.0f);

	//sun starts straight up, at noon
	sun_direction.x = sinf(angle);
	sun_direction.y = cosf(angle);

	//keeps the sun from being perfectly centered overhead
	//todo, check if 5.0f is an actual good angle here
	sun_direction = glm::rotate(glm::mat4(1.0f), 5.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * sun_direction;

	return sun_direction;
}

glm::vec4 LightManager::getSunlightIntensity() const{
	return sunlight_interpolator.Interpolate(sun_timer.GetAlpha());
}

void LightManager::setPointLightIntensity(int light_index, const glm::vec4 &intensity){
	light_intensities[light_index] = intensity;
}

glm::vec4 LightManager::getBackgroundColor() const{
	return background_interpolator.Interpolate(sun_timer.GetAlpha());
}