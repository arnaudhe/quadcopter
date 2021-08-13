#include <utils/pid.h>
#include <esp_attr.h>
#include <hal/log.h>

#define PID_LOWPASS_CUTOFF_FREQUENCY    40.0
#define PID_INTEGRATE_LIMIT             0.1f
#define PID_DEBUG_PERIOD                20

Pid::Pid(float period, float kp, float ki, float kd, float kff, bool debug_enable)
{
    _setpoint           = 0.0f;
    _previous           = 0.0f;
    _integrate          = 0.0f;
    _period             = period;
    _kp                 = kp;
    _ki                 = ki;
    _kd                 = kd;
    _kff                = kff;
    _dterm_filter       = new PT2Filter(period, PID_LOWPASS_CUTOFF_FREQUENCY);
    _debug_ticks        = 0;
    _debug_enable       = debug_enable;

    _dterm_filter->init();
}

float IRAM_ATTR Pid::update(float input)
{
    float error           = _setpoint - input;
    float derivate        = (error - _previous) / (_period);
    float derivate_filter = _dterm_filter->apply(derivate);

    _integrate = _integrate + (error * _period);

    if (_integrate > PID_INTEGRATE_LIMIT)
    {
        _integrate = PID_INTEGRATE_LIMIT;
    }
    else if (_integrate < -PID_INTEGRATE_LIMIT)
    {
        _integrate = -PID_INTEGRATE_LIMIT;
    }

    float output = _kp  * error +
                   _ki  * _integrate +
                   _kd  * derivate_filter +
                   _kff * _setpoint;

    if ((_debug_enable) && ((xTaskGetTickCount() - _debug_ticks) >= PID_DEBUG_PERIOD))
    {
        _debug_ticks = xTaskGetTickCount();
        LOG_DEBUG("%f;%f;%f;%f;%f;%f", _setpoint, input, _kp * error, _ki * _integrate, _kd * derivate_filter, _kff * _setpoint);
    }

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

float IRAM_ATTR Pid::setpoint() const
{
    return _setpoint;
}
