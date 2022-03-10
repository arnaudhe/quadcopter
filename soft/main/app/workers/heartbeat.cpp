#include <app/workers/heartbeat.h>
#include <platform.h>

Heartbeat::Heartbeat(float period, RadioBroker * radio, UdpServer * udp, Motor * motor):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _radio = radio;
    _udp = udp;
    _motor = motor;

    /* Register uplink heartbeat for range test */ 
    _radio->register_channel(HEARTBEAT_CHANNEL);
}

void Heartbeat::run()
{
    uint8_t heartbeat[16];
    uint8_t heartbeat_length = 0;

    _udp->send_broadcast("kwadcopter", PLATFORM_UDP_PORT_BASE + HEARTBEAT_CHANNEL);
    _radio->send(HEARTBEAT_CHANNEL, (uint8_t *)"kwadcopter", 10);

    /* Range test */
    if (_radio->received_frame_pending(HEARTBEAT_CHANNEL))
    {
        if (_radio->receive(HEARTBEAT_CHANNEL, heartbeat, &heartbeat_length))
        {
            _motor->beep();
        }
    }
}
