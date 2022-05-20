#pragma once

#include <drv/ADS1115.hpp>

class Battery
{

private:

    ADS1115 * _ads;
    int       _channel;

public:

    Battery(ADS1115 * ads, int channel);

    float get_voltage(void);

    float get_level_percent(void);
};