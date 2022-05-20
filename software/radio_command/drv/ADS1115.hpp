#pragma once

#include <periph/i2c.hpp>

class ADS1115
{

private:

    I2C * _i2c;

public:

    ADS1115(I2C * i2c);

    int sample_channel(int channel);

};
