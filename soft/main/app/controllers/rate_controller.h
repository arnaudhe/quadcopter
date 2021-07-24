#pragma once

#include <utils/mixer.h>
#include <utils/pid.h>
#include <utils/filter.h>

#include <hal/marg.h>

#include <periph/timer.h>

#include <os/periodic_task.h>
#include <os/mutex.h>

#include <app/observers/euler_observer.h>
#include <app/observers/height_observer.h>

#include <data_model/data_ressources_registry.h>

class RateController : public PeriodicTask
{

private:

    Marg                    * _marg;
    Pid                     * _roll_controller;
    Pid                     * _pitch_controller;
    Pid                     * _yaw_controller;
    Mixer                   * _mixer;
    Mutex                   * _mutex;
    BiQuadraticNotchFilter  * _roll_filter;
    BiQuadraticNotchFilter  * _pitch_filter;
    BiQuadraticNotchFilter  * _yaw_filter;
    float                     _period;

    float _roll_target;
    float _pitch_target;
    float _yaw_target;

    float _roll_calib;
    float _pitch_calib;
    float _yaw_calib;

    float _roll_speed;
    float _pitch_speed;
    float _yaw_speed;

    float _thurst;

    bool _roll_enable;
    bool _pitch_enable;
    bool _yaw_enable;

    void run(void);

public:

    RateController(float period, Marg * marg, Mixer * mixer);

    void set_speed_targets(float roll, float pitch, float yaw);
    void set_enables(float roll, float pitch, float yaw);

    void set_thurst(float thurst);

    float get_roll_speed(void);
    float get_pitch_speed(void);
    float get_yaw_speed(void);
};