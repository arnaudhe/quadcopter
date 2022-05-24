#pragma once

#include <periph/i2c.hpp>

class ADS1115
{

private:

    I2C * _i2c;

    bool _start_conversion(uint16_t channel);

    bool _is_busy();

    bool _is_ready();

    bool _write_register(uint8_t reg_addr, uint16_t value);

    bool _read_register(uint8_t reg_addr, uint16_t * value);

public:

    ADS1115(I2C * i2c);

    int sample_channel(int channel);

};
