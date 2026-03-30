/* switchfl1p 2025-2026 */

//see https://github.com/switchfl1p/tween

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <Timer.hpp>

Timer::Timer(Type t_type, float duration)
    :etype(t_type),
     sec_duration(duration),
     has_updated(false),
     is_paused(false),
     abs_prev_time(0.0f),
     sec_accum_time(0.0f)
{
    if(etype != TT_INFINITE)
        assert(sec_duration > 0.0f);
}

void Timer::reset(){
    has_updated = false;
    sec_accum_time = 0.0f;
}

bool Timer::togglePause(){
    is_paused = !is_paused;
    return is_paused;
}

bool Timer::isPaused() const
{
    return is_paused;
}

void Timer::setPause(bool pause){
    is_paused = pause;
}

bool Timer::update(){
    float abs_curr_time = glfwGetTime();

    //for the first timestamp
    if(!has_updated){
        abs_prev_time = abs_curr_time;
        has_updated = true;
    }

    if(is_paused){
        abs_prev_time = abs_curr_time;
        return false;
    }

    float delta_time = abs_curr_time - abs_prev_time;
    sec_accum_time += delta_time;

    abs_prev_time = abs_curr_time;

    if(etype == TT_SINGLE)
        return sec_accum_time > sec_duration;

    return false;
}

void Timer::rewind(float sec_rewind){
    sec_accum_time -= sec_rewind;

    if(sec_accum_time < 0.0f)
        sec_accum_time = 0.0f;
}

void Timer::fastForward(float sec_ff){
    sec_accum_time += sec_ff;
}

float Timer::getAlpha() const{
    switch(etype){
        case TT_LOOP:
            return fmodf(sec_accum_time, sec_duration) / sec_duration;
        case TT_SINGLE:
            return glm::clamp(sec_accum_time / sec_duration, 0.0f, 1.0f);
        case TT_INFINITE:
        case TIMER_TYPES_COUNT:
            return -1; //garbage
    }
    return -1; //garbage
}

float Timer::getProgression() const{
    switch(etype){
        case TT_LOOP:
            return fmodf(sec_accum_time, sec_duration);
        case TT_SINGLE:
            return glm::clamp(sec_accum_time, 0.0f, sec_duration);
        case TT_INFINITE: 
        case TIMER_TYPES_COUNT:
            return -1; //garbage
    }
    return -1; //garbage
}

float Timer::getTimeSinceStart() const{
    return sec_accum_time;
}

float Timer::getDuration() const{
    return sec_duration;
}