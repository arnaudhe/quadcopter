#include "ADS1115.hpp"

#define ADS1115_SLAVE_ADDR      0x49

ADS1115::ADS1115(I2C * i2c) :
    _i2c(i2c)
{
}

int ADS1115::sample_channel(int channel)
{
    uint8_t msb, lsb;

    _i2c->read_register(ADS1115_SLAVE_ADDR, 2 * channel, &msb);
    _i2c->read_register(ADS1115_SLAVE_ADDR, 2 * channel + 1, &lsb);

    return (msb << 8) + lsb;
}
