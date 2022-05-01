#include <utils/pid.h>
#include <esp_attr.h>
#include <hal/log.h>

#define PID_LOWPASS_CUTOFF_FREQUENCY    10.0

Pid::Pid(float period, float kp, float ki, float kd, float kff, float kt, float min_command, float max_command)
{
    _setpoint           = 0.0f;
    _previous           = 0.0f;
    _integrate          = 0.0f;
    _windup             = 0.0f;
    _period             = period;
    _kp                 = kp;
    _ki                 = ki;
    _kd                 = kd;
    _kff                = kff;
    _kt                 = kt;
    _min_command        = min_command;
    _max_command        = max_command;
    _dterm_filter       = new PT3Filter(period, PID_LOWPASS_CUTOFF_FREQUENCY);

    _dterm_filter->init();
}

float IRAM_ATTR Pid::update(float input)
{
    float output;
    float output_sat;
    float error;
    float proportional;
    float integrate;
    float derivate;
    float feed_forward;

    /* Compute error */
    error = _setpoint - input;

    /* Compute PID terms */

    /* Standard proportional */
    proportional = _kp  * error;
    /* Integrate with anti-windup */
    integrate = _integrate + ((_ki * error) + (_kt * _windup)) * _period;
    /* Derivate with low-pass filtering */
    derivate = _kd * _dterm_filter->apply((_previous - input) / (_period));
    /* Feed-forward */
    feed_forward = _kff * _setpoint;

    /* Compute output command */
    output = proportional + integrate + derivate + feed_forward;

    if (output > _max_command)
    {
        output_sat = _max_command;
    }
    else if (output < _min_command)
    {
        output_sat = _min_command;
    }
    else
    {
        output_sat = output;
    }

    _previous = input;
    _integrate = integrate;
    _windup = output_sat - output;

    return output_sat;
}

void IRAM_ATTR Pid::reset(void)
{
    _previous  = 0.0f;
    _integrate = 0.0f;
    _windup    = 0.0f;
}

void IRAM_ATTR Pid::set_kp(float kp)
{
    _kp = kp;
}

void IRAM_ATTR Pid::set_ki(float ki)
{
    _ki = ki;
}

void IRAM_ATTR Pid::set_kd(float kd)
{
    _kd = kd;
}

void IRAM_ATTR Pid::set_kff(float kff)
{
    _kff = kff;
}

void IRAM_ATTR Pid::set_kt(float kt)
{
    _kt = kt;
}

void IRAM_ATTR Pid::set_setpoint(float setpoint)
{
    _setpoint = setpoint;
}

float IRAM_ATTR Pid::kp() const
{
    return _kp;
}

float IRAM_ATTR Pid::ki() const
{
    return _ki;
}

float IRAM_ATTR Pid::kd() const
{
    return _kd;
}

float IRAM_ATTR Pid::kff() const
{
    return _kff;
}

float IRAM_ATTR Pid::kt() const
{
    return _kt;
}

float IRAM_ATTR Pid::setpoint() const
{
    return _setpoint;
}
