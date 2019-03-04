#include <utils/controller.h>

Controller::Controller(float period, Pid * pid_speed, Pid * pid_position)
{
    _period       = period;
    _set_point    = 0.0f;
    _command      = 0.0f;
    _mode         = Mode::DISABLED;
    _pid_speed    = pid_speed;
    _pid_position = pid_position;
}

void Controller::update_target(Mode mode, float value)
{
    _mode = mode;
    _set_point = value;
}

void Controller::update(float position, float speed)
{
    switch (_mode)
    {
        case Mode::SPEED:
            _command = _pid_speed->update(speed);
            break;

        case Mode::POSITION:
            _command = _pid_position->update(position);

        case Mode::DISABLED:
        default:
            _command = 0.0f;
            break;
    }
}

float Controller::command(void)
{
    return _command;
}