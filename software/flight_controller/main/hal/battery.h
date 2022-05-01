#pragma once

#include <periph/adc.h>

class Battery
{

private:

    Adc          * _adc;
    Adc::Channel   _channel;
    int            _cells;
    float          _measure_divider;

  public:

    Battery(Adc * adc, Adc::Channel channel, int cells, float measure_divider);

    float get_level(void);
};