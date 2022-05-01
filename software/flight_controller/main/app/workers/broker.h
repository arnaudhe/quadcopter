#pragma once

#include <hal/radio.h>
#include <os/mutex.h>
#include <os/periodic_task.h>
#include <os/queue.h>
#include <utils/byte_array.h>
#include <os/udp.h>
#include <string>
#include <tuple>

#define BROKER_REGISTRATION_COUNT    3

using namespace std;

class Broker : public PeriodicTask
{

public:

    enum Channel
    {
        HEARTBEAT     = 0,
        CONTROL       = 1,
        RADIO_COMMAND = 2,
        TELEMETRY     = 3,
        LOGS          = 4,
    };

    enum Medium
    {
        NONE,
        UDP,
        RADIO,
        UDP_AND_RADIO
    };

    struct Registration
    {
        bool       registered;
        Channel    channel;
        Medium     medium;
        Udp      * udp_listener;
        bool       received_frame_pending;
        ByteArray  received_frame;
        Medium     received_frame_medium;
        string     received_frame_address;
    };

private:

    Mutex         * _mutex;
    Radio         * _radio;
    Udp           * _udp_sender;
    Registration    _registrations[BROKER_REGISTRATION_COUNT];

    void run();

public:

    Broker(float period, Radio * radio);

    void register_channel(Channel channel, Medium medium);

    bool received_frame_pending(Channel channel);

    ByteArray receive(Channel channel);

    tuple<ByteArray, Medium, string> receive_from(Channel channel);

    void send(Channel channel, Medium medium, ByteArray data);

    void send_to(Channel channel, ByteArray data, string address);

};