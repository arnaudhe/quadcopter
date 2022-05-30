#pragma once

#include <drv/SI4432.hpp>
#include <stdint.h>
#include <tuple>
#include <utils/byte_array.hpp>

class Radio
{

private:

    Si4432 * _transceiver;
    uint8_t  _address;

public:

    Radio(Si4432 * si4432, uint8_t remote_address);

    void send(uint8_t channel, ByteArray payload);

    tuple<uint8_t, ByteArray> receive(void);

};