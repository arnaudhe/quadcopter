#pragma once

#include <periph/i2c_master_defs.h>
#include <driver/gpio.h>

static const i2c_master_conf_t I2C_MASTER_CONF[I2C_NUM_MAX] = 
{
    [I2C_NUM_0] = 
    {
        .mapping = 
        {
            .sda_io_num = (gpio_num_t)25,
            .scl_io_num = (gpio_num_t)26
        },
        .frequency = 100000
    },
    [I2C_NUM_1] = {
        .mapping = 
        {
            .sda_io_num = (gpio_num_t)18,
            .scl_io_num = (gpio_num_t)19
        },
        .frequency = 200000
    }
};

