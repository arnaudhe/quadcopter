#pragma once

#include <utils/pid.h>

class Controller
{

public:

    enum Mode
    {
        DISABLED,
        SPEED,
        POSITION
    };

private:

    float   _period;
    float   _target;
    float   _command;
    Mode    _mode;
    Pid   * _pid_speed;
    Pid   * _pid_position;

public:

    Controller(float period, Pid * pid_speed, Pid * pid_position);

    void set_pid_coeffs(Mode mode, float kp, float ki, float kd);

    void update_target(Mode mode, float value);

    void update(float position, float speed);

    float command(void);

};