#include <app/workers/heartbeat.h>

#define HEARTBEAT_UDP_PORT     5000

Heartbeat::Heartbeat(float period, Si4432 * si4432, UdpServer * udp):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _si4432 = si4432;
    _udp = udp;
}

void Heartbeat::run()
{
    _udp->send_broadcast("kwadcopter", HEARTBEAT_UDP_PORT);
    _si4432->send_packet((uint8_t *)"kwadcopter", 11);
}
