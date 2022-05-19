#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "i2c.hpp"
#include "spi.hpp"
#include "gpio.hpp"

#define I2C_SLAVE_ADDR      0x49
#define I2C_WHOIAM_REG      0x01
#define I2C_WHOIAM_VALUE    0x85

int main(int argc, char ** argv)
{
    Spi     spi    = Spi();
    I2C     i2c    = I2C();
    Gpio    button = Gpio(27, Gpio::OUTPUT);
    uint8_t value;
    bool    state;

    i2c.open("/dev/i2c-1");
    i2c.read_register(I2C_SLAVE_ADDR, I2C_WHOIAM_REG, &value);
    i2c.close();

    printf("WHOIAM %02x %02x\n", value, I2C_WHOIAM_VALUE);

    spi.open("/dev/spidev0.0", Spi::MODE_0, 250000);
    spi.read_register(0x00, &value);
    spi.close();

    printf("WHOIAM %02x\n", value);

    state = button.read();

    printf("button %d\n", state);

    return 0;
}
