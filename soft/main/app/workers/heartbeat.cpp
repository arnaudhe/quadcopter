#include <app/workers/heartbeat.h>
#include <platform.h>
#include <os/task.h>

Heartbeat::Heartbeat(float period, Broker * broker, Motor * motor):
    PeriodicTask("camera_controller", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _broker = broker;
    _motor  = motor;

    /* Register uplink heartbeat for range test */ 
    _broker->register_channel(Broker::Channel::HEARTBEAT, Broker::Medium::RADIO);
}

void Heartbeat::run()
{
    _broker->send(Broker::Channel::HEARTBEAT, Broker::Medium::UDP_AND_RADIO, ByteArray("kwadcopter"));

    /* Range test */
    if (_broker->received_frame_pending(Broker::Channel::HEARTBEAT))
    {
        ByteArray packet = _broker->receive(Broker::Channel::HEARTBEAT);
        if (packet.length())
        {
            _motor->beep();
        }
    }
}
