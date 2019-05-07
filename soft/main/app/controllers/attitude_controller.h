#pragma once

#include <utils/controller.h>
#include <utils/mixer.h>

#include <hal/marg.h>
#include <hal/barometer.h>
#include <hal/ultrasound.h>

#include <periph/i2c_master.h>
#include <periph/timer.h>

#include <os/periodic_task.h>

#include <app/observers/euler_observer.h>
#include <app/observers/height_observer.h>

#include <data_model/data_ressources_registry.h>

class AttitudeController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    Timer                   * _timer;
    I2cMaster               * _i2c;
    Marg                    * _marg;
    Barometer               * _baro;
    UltrasoundSensor        * _ultrasound;
    HeightObserver          * _height_observer;
    EulerObserver           * _euler_observer;
    Controller              * _height_controller;
    Controller              * _roll_controller;
    Controller              * _pitch_controller;
    Controller              * _yaw_controller;
    Mixer                   * _mixer;
    double                    _period;

    double            _roll_speed;
    double            _pitch_speed;
    double            _yaw_speed;
    double            _height_speed;

    void run(void);

public:

    AttitudeController(double period, DataRessourcesRegistry * registry);

    void set_height_target(Controller::Mode mode, float target);
    void set_roll_target(Controller::Mode mode, float target);
    void set_pitch_target(Controller::Mode mode, float target);
    void set_yaw_target(Controller::Mode mode, float target);

};