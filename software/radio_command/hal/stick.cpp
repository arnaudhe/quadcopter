#include "stick.hpp"

Stick::Stick(int channel, int center_value, int deviation):
    _adc(channel), _center_value(center_value), _deviation(deviation)
{
}

float Stick::get_state(void)
{
    return (float)(_adc.get_value() - this->_center_value) / (this->_deviation);
}
