#pragma once

#include <drv/SI4432.h>
#include <os/mutex.h>
#include <stdint.h>
#include <tuple>
#include <utils/byte_array.h>

class Radio
{

private:

    Si4432 * _transceiver;
    uint8_t  _address;
    Mutex  * _mutex;

public:

    Radio(Si4432 * si4432);

    void send(uint8_t channel, ByteArray payload);

    tuple<uint8_t, ByteArray> receive(void);

};