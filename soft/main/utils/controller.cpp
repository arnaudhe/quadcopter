#include <utils/controller.h>

Controller::Controller(float period, Pid * pid_speed, Pid * pid_position)
{
    _period       = period;
    _target       = 0.0f;
    _command      = 0.0f;
    _mode         = Mode::DISABLED;
    _pid_speed    = pid_speed;
    _pid_position = pid_position;
}

void Controller::update_target(Mode mode, float value)
{
    _mode = mode;
    _target = value;

    switch (_mode)
    {
        case Mode::SPEED:
            _pid_speed->set_consign(value);
            break;

        case Mode::POSITION:
            _pid_position->set_consign(value);
            break;

        case Mode::DISABLED:
        default:
            break;
    }
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
            break;

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