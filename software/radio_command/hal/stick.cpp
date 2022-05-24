#include "stick.hpp"

#include <math.h>

Stick::Stick(int channel, int center_value, int deviation, float sens, float rate, float expo, float acro):
    _adc(channel), _center_value(center_value), _deviation(deviation), _sens(sens), _rate(rate), _expo(expo), _acro(acro)
{
}

float Stick::get_command(void)
{
    float normalized = (float)(_adc.get_value() - this->_center_value) / (this->_deviation);
    float command    = _sens * ((1 + 0.01 * _expo * (normalized * normalized - 1.0f)) * normalized);
    return (command * (_rate + (fabs(command) * _rate * _acro * 0.01f)));
}
