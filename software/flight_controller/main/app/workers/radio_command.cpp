#include <app/workers/radio_command.h>
#include <hal/log.h>
#include <string.h>
#include <platform.h>

#define RADIO_COMMAND_FAILSAFE_TIMEOUT      1500

typedef struct
{
    uint8_t armed;
    float   roll;
    float   pitch;
    float   yaw;
    float   throttle;
} __attribute__((packed)) RadioCommandPacket;

RadioCommand::RadioCommand(DataRessourcesRegistry * registry, Broker * broker) :
    Task("rc", Task::Priority::MEDIUM, 4096, false)
{
    _registry = registry;
    _broker   = broker;

    _broker->register_channel(Broker::Channel::RADIO_COMMAND, Broker::Medium::UDP_AND_RADIO);

    LOG_INFO("Radio channel registered");
}

void RadioCommand::run()
{
    ByteArray            packet;
    RadioCommandPacket * rc_packet;
    TickType_t           last_command_tick = 0;

    while (1)
    {
        if (_broker->received_frame_pending(Broker::Channel::RADIO_COMMAND))
        {
            /* Get the received radio frame */
            packet = _broker->receive(Broker::Channel::RADIO_COMMAND);
        }

        if (packet.length() == sizeof(RadioCommandPacket))
        {
            last_command_tick = xTaskGetTickCount();

            rc_packet = (RadioCommandPacket *)(packet.data());

            _registry->internal_set<string>("control.mode", rc_packet->armed ? "attitude" : "off");
            _registry->internal_set<float>("control.attitude.roll.position.target", rc_packet->roll);
            _registry->internal_set<float>("control.attitude.pitch.position.target", rc_packet->pitch);
            _registry->internal_set<float>("control.attitude.height.speed.target", rc_packet->throttle);
            _registry->internal_set<float>("control.attitude.yaw.speed.target", rc_packet->yaw);
        }
        else if ((last_command_tick > 0) && ((TickType_t)(xTaskGetTickCount() - last_command_tick) > RADIO_COMMAND_FAILSAFE_TIMEOUT))
        {
            LOG_ERROR("Signal Lost ! Set failsafe commands");

            last_command_tick = xTaskGetTickCount();

            if (_registry->internal_get<string>("control.mode") == "attitude")
            {
                _registry->internal_set<float>("control.attitude.roll.position.target",  0.0);
                _registry->internal_set<float>("control.attitude.pitch.position.target", 0.0);
                _registry->internal_set<float>("control.attitude.height.speed.target", -0.5);
                _registry->internal_set<float>("control.attitude.yaw.speed.target", 0.0);
            }
        }

        Task::delay_ms(50);
    }
}