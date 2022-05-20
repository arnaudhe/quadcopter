#include "adc.hpp"

Adc::Adc(int channel)
{
    _channel = channel;
}

int Adc::get_value(void)
{
    return 1000;
}
