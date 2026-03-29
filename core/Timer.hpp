/* switchfl1p 2025-2026 */
//see https://github.com/switchfl1p/tween

#pragma once

class Timer{
public:
    enum Type{
        TT_LOOP,
        TT_SINGLE,
        TT_INFINITE,

        TIMER_TYPES_COUNT
    };

    //Infinite timers ignore the duration
    Timer(Type t_type = TT_INFINITE, float duration = 1.0f);

    void reset();

    //returns true if the timer is paused after toggling
    bool togglePause();

    void setPause(bool pause = true);

    bool isPaused() const;

    //Updates the time for the timer. Returns true if the timer has reached the end
    //Will only return true for single timers
    bool update();

    //subtracts sec_rewind from current time and continues from there
    void rewind(float sec_rewind);

    //adds sec_ff to the current time and continues from there
    void fastForward(float sec_ff);

    //returns a value between [0, 1] representing progress through the duration.
    //Only used for SINGLE and LOOP timers.
    float getAlpha() const;

    //returns a number between [0, duration] reprenseting the progress through the timer in seconds
    //Only for SINGLE and LOOP timers
    float getProgression() const;

    //returns the time in seconds since the timer started, excluding time spent paused
    float getTimeSinceStart() const;

    //returns the timer's duration that was passed in
    float getDuration() const;

private:
    Type etype;
    float sec_duration;

    bool has_updated;
    bool is_paused;

    float abs_prev_time;
    float sec_accum_time;
};