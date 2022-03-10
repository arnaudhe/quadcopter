#include <app/workers/heartbeat.h>
#include <platform.h>

Heartbeat::Heartbeat(float period, RadioBroker * radio, UdpServer * udp):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _radio = radio;
    _udp = udp;
}

void Heartbeat::run()
{
    _udp->send_broadcast("kwadcopter", PLATFORM_UDP_PORT_BASE + HEARTBEAT_CHANNEL);
    _radio->send(HEARTBEAT_CHANNEL, (uint8_t *)"kwadcopter", 10);
}
