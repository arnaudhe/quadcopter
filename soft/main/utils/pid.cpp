#include <utils/pid.h>

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

float Pid::update(float input)
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

void Pid::set_kp(float kp)
{
    _kp = kp;
}

void Pid::set_ki(float ki)
{
    _ki = ki;
}

void Pid::set_kd(float kd)
{
    _kd = kd;
} 

void Pid::set_consign(float consign)
{
    _consign = consign;
} 

float Pid::kp() const
{
    return _kp;
}

float Pid::ki() const
{
    return _ki;
}

float Pid::kd() const
{
    return _kd;
}

float Pid::consign() const
{
    return _consign;
}
