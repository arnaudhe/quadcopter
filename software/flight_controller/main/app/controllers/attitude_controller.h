#pragma once

#include <utils/mixer.h>
#include <utils/pid.h>
#include <utils/filter.h>

#include <hal/marg.h>
#include <hal/barometer.h>
#include <hal/ultrasound.h>

#include <periph/i2c_master.h>
#include <periph/timer.h>

#include <os/periodic_task.h>
#include <os/mutex.h>

#include <app/observers/euler_observer.h>
#include <app/observers/height_observer.h>
#include <app/controllers/rate_controller.h>

#include <data_model/data_ressources_registry.h>

class AttitudeController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    RateController          * _rate_controller;
    Marg                    * _marg;
    EulerObserver           * _observer;
    Pid                     * _height_controller;
    Pid                     * _roll_controller;
    Pid                     * _pitch_controller;
    Pid                     * _yaw_controller;
    Mixer                   * _mixer;
    Mutex                   * _mutex;
    SlewFilter              * _roll_command_filter;
    SlewFilter              * _pitch_command_filter;
    SlewFilter              * _yaw_command_filter;
    float                     _period;

    void run(void);

public:

    AttitudeController(float period, DataRessourcesRegistry * registry, RateController * rate_controller, Marg * marg);

    void rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r);
};