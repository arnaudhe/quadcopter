#pragma once

#include <utils/controller.h>
#include <utils/attitude.h>
#include <utils/mixer.h>

class AttitudeController
{

private:

    Attitude   * _attitude;
    Controller * _height_controller;
    Controller * _roll_controller;
    Controller * _pitch_controller;
    Controller * _yaw_controller;
    Mixer      * _mixer;
    float        _period;

public:

    AttitudeController(float period);

    void update(void);

    void set_height_target(Controller::Mode mode, float target);
    void set_roll_target(Controller::Mode mode, float target);
    void set_pitch_target(Controller::Mode mode, float target);
    void set_yaw_target(Controller::Mode mode, float target);

};