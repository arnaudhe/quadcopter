#pragma once

#include <stdint.h>

class Crc16
{

private:

    uint16_t _crc = 0;

public:

    Crc16(uint16_t initial_crc = 0);

    uint16_t update(const uint8_t * data, int length);

    uint16_t get_crc(void);

    uint8_t get_msb(void);

    uint8_t get_lsb(void);
};