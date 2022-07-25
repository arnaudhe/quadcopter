#include "ADS1115.hpp"
#include "ADS1115_conf.hpp"
#include "ADS1115_defs.hpp"

#include <unistd.h>
#include <iostream>

#define ADS1115_SLAVE_ADDR      0x49

ADS1115::ADS1115(I2C * i2c) :
    _i2c(i2c)
{
    uint16_t config;

   _read_register(ADS1X15_REG_CONFIG, &config);

    printf("default_config %04x\n", config);
}

bool ADS1115::_write_register(uint8_t reg_addr, uint16_t value)
{
    uint8_t buffer[2];

    buffer[0] = value >> 8;
    buffer[1] = value & 0xff;

    return _i2c->write_registers(ADS1115_SLAVE_ADDR, reg_addr, 2, buffer);
}

bool ADS1115::_read_register(uint8_t reg_addr, uint16_t * value)
{
    uint8_t buffer[2];

    if (_i2c->read_registers(ADS1115_SLAVE_ADDR, reg_addr, 2, buffer) == false)
    {
        *value = 0;
        return false;
    }

    *value = (buffer[0] << 8) + buffer[1];

    return true;
}

bool ADS1115::_start_conversion(uint16_t channel)
{
    uint16_t config = 0;

    config |= ADS1X15_OS_START_SINGLE;          // bit 15     force wake up if needed
    config |= ((4 + channel) << 12);            // bit 12-14
    config |= ADS1115_CONF_GAIN;                // bit 9-11
    config |= ADS1115_CONF_MODE;                // bit 8
    config |= ADS1115_CONF_DATARATE;            // bit 5-7
    config |= ADS1X15_COMP_MODE_TRADITIONAL;
    config |= ADS1X15_COMP_POL_ACTIV_LOW;
    config |= ADS1X15_COMP_NON_LATCH;           // bit 2      ALERT latching
    config |= 3;                                // bit 0..1   ALERT mode

    return _write_register(ADS1X15_REG_CONFIG, config);
}

bool ADS1115::_is_busy()
{
    return (_is_ready() == false);
}

bool ADS1115::_is_ready()
{
    uint16_t config = 0x00;

    if (_read_register(ADS1X15_REG_CONFIG, &config) == false)
    {
        return false;
    }

    return (bool)(config & ADS1X15_OS_NOT_BUSY);
}

int ADS1115::sample_channel(int channel)
{
    uint16_t sample;

    if (channel >= 4)
    {
        return 0;
    }

    if (_start_conversion(channel) == false)
    {
        std::cerr << "Failed to start conversion" << std::endl;
        return 0;
    }

    while (_is_busy())
    {
        usleep(1000);
    }

    if (_read_register(ADS1X15_REG_CONVERT, &sample) == false)
    {
        std::cerr << "Failed to read sample" << std::endl;
        return 0;
    }

    return sample;
}
