#pragma once

#include <periph/adc.hpp>

class Stick
{

private:

    Adc     _adc;
    int     _center_value;
    int     _deviation;
    float   _sens;
    float   _rate;
    float   _expo;
    float   _acro;

public:

    Stick(int adc_channel, int center_value, int deviation, float sens, float rate, float expo, float acro);

    float get_command(void);

};
