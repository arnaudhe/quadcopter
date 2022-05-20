#pragma once

#include <periph/adc.hpp>

class Stick
{

private:

    Adc _adc;
    int _center_value;
    int _deviation;

public:

    Stick(int adc_channel, int center_value, int deviation);

    float get_state(void);

};
