#include <utils/pid.h>
#include <esp_attr.h>

Pid::Pid(float period, float kp, float ki, float kd) 
{
    _consign   = 0;
    _previous  = 0;
    _integrate = 0;
    _period    = period;
    _kp        = kp;
    _ki        = ki;
    _kd        = kd;
}

float IRAM_ATTR Pid::update(float input)
{
    float error     = _consign - input;
    float derivate  = (error - _previous) / (_period);
    float integrate = _integrate + (error * _period);

    float output    = _kp * error +
                      _ki * integrate +
                      _kd * derivate;

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

float IRAM_ATTR Pid::consign() const
{
    return _consign;
}
