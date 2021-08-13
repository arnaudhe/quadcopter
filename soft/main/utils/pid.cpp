#include <utils/pid.h>
#include <esp_attr.h>

#define PID_LOWPASS_CUTOFF_FREQUENCY    40.0

Pid::Pid(float period, float kp, float ki, float kd, float kff)
{
    _consign            = 0.0f;
    _previous           = 0.0f;
    _integrate          = 0.0f;
    _period             = period;
    _kp                 = kp;
    _ki                 = ki;
    _kd                 = kd;
    _kff                = kff;
    _dterm_filter       = new PT2Filter(period, PID_LOWPASS_CUTOFF_FREQUENCY);

    _dterm_filter->init();
}

float IRAM_ATTR Pid::update(float input)
{
    float error           = _consign - input;
    float derivate        = (error - _previous) / (_period);
    float integrate       = _integrate + (error * _period);
    float derivate_filter = _dterm_filter->apply(derivate);

    float output = _kp  * error +
                   _ki  * _integrate +
                   _kd  * derivate_filter +
                   _kff * _consign;

    _integrate = integrate;
    _previous = error;

    return output;
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

void IRAM_ATTR Pid::set_consign(float consign)
{
    _consign = consign;
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

float IRAM_ATTR Pid::consign() const
{
    return _consign;
}
