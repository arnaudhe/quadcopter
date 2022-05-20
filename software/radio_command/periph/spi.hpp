#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

class Spi
{

public:

    enum Mode {
        MODE_0 = SPI_MODE_0,
        MODE_1 = SPI_MODE_1,
        MODE_2 = SPI_MODE_2,
        MODE_3 = SPI_MODE_3,
    };

private:

    int         fd;
    Mode        mode;
    uint8_t     bits;
    uint32_t    speed;
    uint16_t    delay;

    bool _transfer(uint8_t * tx, uint8_t * rx, size_t len);

public:

    bool open(const char * port, Mode mode, uint32_t speed_hz);

    bool read_register(uint8_t reg_addr, uint8_t * value);

    bool write_register(uint8_t reg_addr, uint8_t value);

    bool read_registers(uint8_t reg_addr, size_t len, uint8_t * value);

    bool write_registers(uint8_t reg_addr, size_t len, uint8_t * value);

    bool close(void);

};
