#pragma once

#include <hal/motor.h>

class Mixer
{

private:

    Motor * _front_left;
    Motor * _front_right;
    Motor * _rear_left;
    Motor * _rear_right;

public:

    Mixer(Motor * front_left, Motor * front_right, Motor * rear_left, Motor * rear_right);

    void update(float height_command, float roll_command, float pitch_command, float yaw_command);

    void set_motors_speeds(float front_left, float front_right, float rear_left, float rear_right);

};