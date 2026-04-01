/* switchfl1p 2025-2026 */

#include <glm/ext/matrix_transform.hpp>
#include <LightManager.hpp>
#include <InterpolatorTraits.hpp>

//sun timer no longer passed on initalization
LightManager::LightManager(std::span<ConstVelLinearInterpolator<glm::vec3>> interpolators, std::span<Timer> timers) {
	pl_interpolators.assign(interpolators.begin(), interpolators.end());
	pl_timers.assign(timers.begin(), timers.end());
    pl_intensities.resize(NUMBER_OF_POINT_LIGHTS, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
}

void LightManager::setDirLightValues(std::span<std::span<DirLightValue>> dir_lights) {
	for (int i = 0; i < dir_lights.size(); i++) {
		std::vector<std::pair<glm::vec4, float>> intensities;
		std::vector<std::pair<glm::vec4, float>> ambient_intensities;
		std::vector<std::pair<glm::vec4, float>> background_intensities;
		std::vector<std::pair<float, float>> max_intensities;

		for (auto& value : dir_lights[i]) {
			intensities.push_back(std::pair<glm::vec4, float>(value.dl_intensity, value.norm_time));
			ambient_intensities.push_back(std::pair<glm::vec4, float>(value.ambient, value.norm_time));
			background_intensities.push_back(std::pair<glm::vec4, float>(value.background_color, value.norm_time));
			max_intensities.push_back(std::pair<float, float>(value.max_intensity, value.norm_time));
		}

		dl_interpolators[i].setValues(intensities);
		ambient_interpolators[i].setValues(ambient_intensities);
		background_interpolators[i].setValues(background_intensities);
		max_intensity_interpolators[i].setValues(max_intensities);
	}
}

void LightManager::updateTime(){
	for (auto& timer : dl_timers) {
		timer.update();
	}

	for (auto& timer : pl_timers) {
		timer.update();
	}

	for (auto& [name, timer] : extra_timers) {
		timer.update();
	}
}

void LightManager::setPause(TimerTypes timer, bool pause){
	if(timer == TIMER_ALL || timer == TIMER_POINT_LIGHTS){
		for(auto& timer : pl_timers){
			timer.setPause(pause);
		}
		
		for(auto& [name, timer] : extra_timers){
			timer.setPause(pause);
		}
	}

	if(timer == TIMER_ALL || timer == TIMER_DIR_LIGHTS){
		for (auto& timer : dl_timers) {
			timer.setPause(pause);
		}
	}
}

void LightManager::togglePause(TimerTypes timer){
	setPause(timer, !isPaused(timer));
}

bool LightManager::isPaused(TimerTypes timer) const{
	if (timer == TIMER_ALL || timer == TIMER_DIR_LIGHTS) {
		return dl_timers.front().isPaused();
	}

	return pl_timers.front().isPaused();
}

void LightManager::rewindTime(TimerTypes timer, float sec_rewind){
	if (timer == TIMER_ALL || timer == TIMER_DIR_LIGHTS) {
		for (auto& timer : dl_timers) {
			timer.rewind(sec_rewind);
		}
	}

	if(timer == TIMER_ALL || timer == TIMER_POINT_LIGHTS){
		for(auto& timer : pl_timers){
			timer.rewind(sec_rewind);
		}

		for(auto& [name, timer] : extra_timers){
			timer.rewind(sec_rewind);
		}
	}
}

void LightManager::fastForwardTime(TimerTypes timer, float secFF){
	if (timer == TIMER_ALL || timer == TIMER_DIR_LIGHTS) {
		for (auto& timer : dl_timers) {
			timer.fastForward(secFF);
		}
	}

	if(timer == TIMER_ALL || timer == TIMER_POINT_LIGHTS){
		for(auto& timer : pl_timers){
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

	AmbientLight ambient;
	ambient.intensity = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float max_intensity;

	for (int i = 0; i < light_data.dir_light_count; i++) {
		ambient.intensity += ambient_interpolators[i].interpolate(dl_timers[i].getAlpha()); 
		max_intensity += max_intensity_interpolators[i].interpolate(dl_timers[i].getAlpha());

		light_data.dir_lights[i].direction = world_to_camera_mat * getDirLightDirection();
		light_data.dir_lights[i].intensity = dl_interpolators[i].interpolate(dl_timers[i].getAlpha());
	}

	light_data.ambient_light = ambient;
	light_data.max_intensity = max_intensity;

	for(int i = 0; i < light_data.point_light_count; i++){
		glm::vec4 world_light_pos = glm::vec4(pl_interpolators[i].interpolate(pl_timers[i].getAlpha()), 1.0f);
		glm::vec4 light_pos_cam_space = world_to_camera_mat * world_light_pos;

		light_data.point_lights[i].position = light_pos_cam_space;
		light_data.point_lights[i].intensity = pl_intensities[i];
	}

	/* 	light_data.ambient_light.intensity = ambient_interpolator.interpolate(sun_timer.getAlpha());
	light_data.max_intensity = max_intensity_interpolator.interpolate(sun_timer.getAlpha());

	light_data.dir_lights[0].direction = world_to_camera_mat * getSunlightDirection();
	light_data.dir_lights[0].intensity = sunlight_interpolator.interpolate(sun_timer.getAlpha());

	glm::vec4 sun_intensity = sunlight_interpolator.interpolate(sun_timer.getAlpha());
	light_data.dir_lights[1].direction = world_to_camera_mat * -getSunlightDirection();
	light_data.dir_lights[1].intensity = (glm::vec4(1.0f) - sun_intensity) * glm::vec4(0.15f, 0.15f, 0.25f, 1.0f);
	light_data.dir_lights[1].intensity.w = 1.0f; */

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