/* switchfl1p 2025-2026 */

#include <glm/ext/matrix_transform.hpp>
#include <LightManager.hpp>
#include <InterpolatorTraits.hpp>

LightManager::LightManager()
    : sun_timer(Timer::TT_LOOP, 30.0f)
{
    light_timers.reserve(NUMBER_OF_POINT_LIGHTS);

    light_pos_interpolators.resize(NUMBER_OF_POINT_LIGHTS, ConstVelLinearInterpolator<glm::vec3>());
    light_intensities.resize(NUMBER_OF_POINT_LIGHTS, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    std::vector<glm::vec3> pos_values;
    pos_values.reserve(60);

	// Light 0 - zigzags and cuts inward, varying heights
	pos_values.push_back(glm::vec3(-5.0f, 1.5f, 8.0f));
	pos_values.push_back(glm::vec3(-2.0f, 3.0f, 5.0f));   // cuts inward, rises
	pos_values.push_back(glm::vec3(-8.0f, 1.0f, 2.0f));   // swings back out, dips
	pos_values.push_back(glm::vec3(-4.0f, 4.0f, -3.0f));  // cuts inward again, peaks
	pos_values.push_back(glm::vec3(-8.0f, 1.0f, -7.0f));  // back to edge, low
	pos_values.push_back(glm::vec3(0.0f,  2.5f, -4.0f));  // cuts across center
	pos_values.push_back(glm::vec3(7.0f,  1.0f, -8.0f));  // swings to far corner
	pos_values.push_back(glm::vec3(4.0f,  3.5f, -2.0f));  // cuts inward, rises
	pos_values.push_back(glm::vec3(8.0f,  1.0f,  3.0f));  // back to edge
	pos_values.push_back(glm::vec3(2.0f,  4.0f,  6.0f));  // cuts inward, high
	pos_values.push_back(glm::vec3(7.0f,  1.0f,  8.0f));  // back to edge, low
	light_pos_interpolators[0].setValues(pos_values);
	light_timers.push_back(Timer(Timer::TT_LOOP, 15.0f));

	// Light 1 - erratic, tight direction changes, big height variance
	pos_values.clear();
	pos_values.push_back(glm::vec3( 3.0f, 4.5f,  2.0f));  // starts high near center
	pos_values.push_back(glm::vec3( 8.0f, 1.0f,  5.0f));  // dives to edge
	pos_values.push_back(glm::vec3( 4.0f, 2.0f, -1.0f));  // cuts back
	pos_values.push_back(glm::vec3( 7.0f, 4.0f, -7.0f));  // far corner, rises
	pos_values.push_back(glm::vec3( 1.0f, 1.0f, -5.0f));  // cuts inward, low
	pos_values.push_back(glm::vec3(-4.0f, 3.5f, -8.0f));  // swings to edge, rises
	pos_values.push_back(glm::vec3(-2.0f, 1.0f, -2.0f));  // cuts to center, dips
	pos_values.push_back(glm::vec3(-8.0f, 4.0f,  1.0f));  // far edge, high
	pos_values.push_back(glm::vec3(-3.0f, 1.5f,  5.0f));  // cuts inward
	pos_values.push_back(glm::vec3( 2.0f, 5.0f,  8.0f));  // peaks high near edge
	pos_values.push_back(glm::vec3(-1.0f, 1.0f,  3.0f));  // drops to near center
	pos_values.push_back(glm::vec3( 6.0f, 3.0f,  1.0f));  // swings out
	light_pos_interpolators[1].setValues(pos_values);
	light_timers.push_back(Timer(Timer::TT_LOOP, 25.0f));

	// Light 2 - figure-8, crosses center, medium height
	pos_values.clear();
	pos_values.push_back(glm::vec3( 7.0f, 2.0f,  5.0f));
	pos_values.push_back(glm::vec3( 4.0f, 2.5f,  8.0f));
	pos_values.push_back(glm::vec3(-1.0f, 2.0f,  5.0f));
	pos_values.push_back(glm::vec3( 0.0f, 1.5f,  0.0f));  // crosses center
	pos_values.push_back(glm::vec3(-1.0f, 2.0f, -5.0f));
	pos_values.push_back(glm::vec3(-4.0f, 2.5f, -8.0f));
	pos_values.push_back(glm::vec3(-7.0f, 2.0f, -5.0f));
	pos_values.push_back(glm::vec3(-4.0f, 1.5f,  0.0f));  // crosses center again
	pos_values.push_back(glm::vec3( 4.0f, 2.0f, -5.0f));
	pos_values.push_back(glm::vec3( 7.0f, 2.5f, -8.0f));
	pos_values.push_back(glm::vec3( 8.0f, 2.0f,  0.0f));
	pos_values.push_back(glm::vec3( 5.0f, 1.5f,  6.0f));
	light_pos_interpolators[2].setValues(pos_values);
	light_timers.push_back(Timer(Timer::TT_LOOP, 35.0f));
}

void LightManager::setSunlightValues(std::span<SunlightValue> sun_values){
	std::vector<std::pair<glm::vec4, float>> ambient;
	std::vector<std::pair<glm::vec4, float>> light;
	std::vector<std::pair<glm::vec4, float>> background;
	std::vector<std::pair<float, float>> max_intensity;

	for(auto& value : sun_values){
		ambient.push_back(std::pair<glm::vec4, float>(value.ambient, value.norm_time));
		light.push_back(std::pair<glm::vec4, float>(value.sunlightIntensity, value.norm_time));
		background.push_back(std::pair<glm::vec4, float>(value.backgroundColor, value.norm_time));
		max_intensity.push_back(std::pair<float, float>(value.max_intensity, value.norm_time));
	}

	ambient_interpolator.setValues(ambient);
	sunlight_interpolator.setValues(light);
	background_interpolator.setValues(background);
	max_intensity_interpolator.setValues(max_intensity);
}

void LightManager::updateTime(){
	sun_timer.update();

	for(auto& timer : light_timers){
		timer.update();
	}
	for(auto& [name, timer] : extra_timers){
		timer.update();
	}
}

void LightManager::setPause(TimerTypes timer, bool pause){
	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.setPause(pause);
		}
		
		for(auto& [name, timer] : extra_timers){
			timer.setPause(pause);
		}
	}

	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.togglePause();
	}
}

void LightManager::togglePause(TimerTypes timer){
	setPause(timer, !isPaused(timer));
}

bool LightManager::isPaused(TimerTypes timer) const{
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		return sun_timer.isPaused();
	}
	return light_timers.front().isPaused();
}

void LightManager::rewindTime(TimerTypes timer, float sec_rewind){
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.rewind(sec_rewind);
	}

	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.rewind(sec_rewind);
		}

		for(auto& [name, timer] : extra_timers){
			timer.rewind(sec_rewind);
		}
	}
}

void LightManager::fastForwardTime(TimerTypes timer, float secFF){
	if(timer == TIMER_ALL || timer == TIMER_SUN){
		sun_timer.fastForward(secFF);
	}

	if(timer == TIMER_ALL || timer == TIMER_LIGHTS){
		for(auto& timer : light_timers){
			timer.fastForward(secFF);
		}

		for(auto& [name, timer] : extra_timers){
			timer.fastForward(secFF);
		}
	}
}

LightBlock LightManager::getLightInformation(const glm::mat4 &world_to_camera_mat) const{
	LightBlock light_data;

	light_data.point_light_count = NUMBER_OF_POINT_LIGHTS;
    light_data.dir_light_count = NUMBER_OF_DIR_LIGHTS;  

	light_data.ambient_light.intensity = ambient_interpolator.interpolate(sun_timer.getAlpha());
	light_data.max_intensity = max_intensity_interpolator.interpolate(sun_timer.getAlpha());

	light_data.dir_lights[0].direction = world_to_camera_mat * getSunlightDirection();
	light_data.dir_lights[0].intensity = sunlight_interpolator.interpolate(sun_timer.getAlpha());

	glm::vec4 sun_intensity = sunlight_interpolator.interpolate(sun_timer.getAlpha());
	light_data.dir_lights[1].direction = world_to_camera_mat * -getSunlightDirection();
	light_data.dir_lights[1].intensity = (glm::vec4(1.0f) - sun_intensity) * glm::vec4(0.15f, 0.15f, 0.25f, 1.0f);
	light_data.dir_lights[1].intensity.w = 1.0f;

	for(int i = 0; i < light_data.point_light_count; i++){
		glm::vec4 world_light_pos = glm::vec4(light_pos_interpolators[i].interpolate(light_timers[i].getAlpha()), 1.0f);
		glm::vec4 light_pos_cam_space = world_to_camera_mat * world_light_pos;

		light_data.point_lights[i].position = light_pos_cam_space;
		light_data.point_lights[i].intensity = light_intensities[i];
	}
	return light_data;
}

//rotates sunlight and returns it
glm::vec4 LightManager::getSunlightDirection() const{
	float angle = 2.0f * 3.14159f * sun_timer.getAlpha();
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
	return sunlight_interpolator.interpolate(sun_timer.getAlpha());
}

void LightManager::setPointLightIntensity(int light_index, const glm::vec4 &intensity){
	light_intensities[light_index] = intensity;
}

glm::vec4 LightManager::getBackgroundColor() const{
	return background_interpolator.interpolate(sun_timer.getAlpha());
}

glm::vec3 LightManager::getWorldLightPosition(int light_index) const{
	return light_pos_interpolators[light_index].interpolate(light_timers[light_index].getAlpha());
}