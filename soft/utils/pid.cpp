#include <utils/pid.h>

Pid::Pid(float period, float kp, float ki, float kd) 
    : _period(period), _kp(kp), _ki(ki), _kd(kd)
{
    this->_consign   = 0;
    this->_previous  = 0;
    this->_integrate = 0;
}

float Pid::update(float input)
{
    float error     = this->_consign - input;
    float derivate  = (input - this->_previous) / (this->_period);
    float integrate = this->_integrate + (error * this->_period);

    float output    = this->_kp * error +
                      this->_ki * integrate + 
                      this->_kd * derivate;

    this->_integrate = integrate;
    this->_previous  = input;

    return output;
}

void Pid::set_kp(float kp)
{
    this->_kp = kp;
}

void Pid::set_ki(float ki)
{
    this->_ki = ki;
}

void Pid::set_kd(float kd)
{
    this->_kd = kd;
} 

void Pid::set_consign(float consign)
{
    this->_consign = consign;
} 

float Pid::kp() const
{
    return this->_kp;
}

float Pid::ki() const
{
    return this->_ki;
}

float Pid::kd() const
{
    return this->_kd;
}

float Pid::consign() const
{
    return this->_consign;
}
