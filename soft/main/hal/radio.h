#pragma once

#include <drv/SI4432.h>
#include <os/mutex.h>
#include <os/periodic_task.h>
#include <os/queue.h>

#define RADIO_REGISTRATION_COUNT    3

class RadioBroker : public PeriodicTask
{

public:

    struct Registration
    {
        bool    registered;
        uint8_t channel;
        bool    received_frame_pending;
        uint8_t received_frame_length;
        uint8_t received_frame[64];
    };

private:

    Si4432          * _transceiver;
    Mutex           * _mutex;
    uint8_t           _address;
    Registration      _registrations[RADIO_REGISTRATION_COUNT];

    void run();

public:

    RadioBroker(float period, uint8_t address, Si4432 * transceiver);

    void register_channel(uint8_t channel);

    bool received_frame_pending(uint8_t channel);

    bool receive(uint8_t channel, uint8_t * data, uint8_t * length);

    void send(uint8_t channel, uint8_t * data, uint8_t length);

};