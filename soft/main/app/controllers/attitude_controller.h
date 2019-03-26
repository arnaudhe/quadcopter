#pragma once

#include <utils/controller.h>
#include <utils/mixer.h>

#include <hal/marg.h>
#include <hal/barometer.h>

#include <periph/i2c_master.h>

#include <app/observers/euler_observer.h>
#include <app/observers/height_observer.h>

class AttitudeController
{

private:

    I2cMaster      * _i2c;
    Marg           * _marg;
    Barometer      * _baro;
    HeightObserver * _height_observer;
    EulerObserver  * _euler_observer;
    Controller     * _height_controller;
    Controller     * _roll_controller;
    Controller     * _pitch_controller;
    Controller     * _yaw_controller;
    Mixer          * _mixer;
    float            _period;

    float            _roll_speed;
    float            _pitch_speed;
    float            _yaw_speed;
    float            _height_speed;

public:

    AttitudeController(float period);

    void update(void);

    void set_height_target(Controller::Mode mode, float target);
    void set_roll_target(Controller::Mode mode, float target);
    void set_pitch_target(Controller::Mode mode, float target);
    void set_yaw_target(Controller::Mode mode, float target);

};