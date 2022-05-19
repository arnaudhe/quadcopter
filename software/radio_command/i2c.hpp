#pragma once

#include <stdbool.h>
#include <stdint.h>

class I2C
{

private:

    int     fd;
    uint8_t slave_addr;

    bool _set_slave_addr(uint8_t slave_addr);

    bool _read_bus(uint8_t * data, size_t len);

    bool _write_bus(uint8_t * data, size_t len);

public:

    bool open(const char * port);

    bool read_register(uint8_t slave_addr, uint8_t reg_addr, uint8_t * value);

    bool write_register(uint8_t slave_addr, uint8_t reg_addr, uint8_t value);

    bool close();

};
