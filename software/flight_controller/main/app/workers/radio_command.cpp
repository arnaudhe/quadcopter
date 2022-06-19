#include <app/workers/radio_command.h>
#include <hal/log.h>
#include <string.h>
#include <platform.h>

#define RADIO_COMMAND_FAILSAFE_TIMEOUT      1500

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
    ByteArray         packet;
    int               rssi = -100;
    CommandsPayload * commands;
    StatusPayload     status;
    TickType_t        last_command_tick = 0;

    while (1)
    {
        for (int i = 0; i < 20; i++)
        {
            if (_broker->received_frame_pending(Broker::Channel::RADIO_COMMAND))
            {
                /* Get the received radio frame */
                tuple<ByteArray, int> recv = _broker->receive_rssi(Broker::Channel::RADIO_COMMAND);
                packet = get<0>(recv);
                rssi   = get<1>(recv);
            }

            if (packet.length() == sizeof(RadioCommand::CommandsPayload))
            {
                last_command_tick = xTaskGetTickCount();

                commands = (CommandsPayload *)(packet.data());

                _registry->internal_set<string>("control.mode", commands->armed ? "attitude" : "off");
                _registry->internal_set<float>("control.attitude.roll.position.target", commands->roll);
                _registry->internal_set<float>("control.attitude.pitch.position.target", commands->pitch);
                _registry->internal_set<float>("control.attitude.height.speed.target", commands->throttle);
                _registry->internal_set<float>("control.attitude.yaw.speed.target", commands->yaw);
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

            if (rssi > -20)
            {
                status.link_quality = 99;
            }
            else if (rssi < -90)
            {
                status.link_quality = 0;
            }
            else
            {
                status.link_quality = ((90 + rssi) * 100) / 70;
            }

            status.armed            = (_registry->internal_get<string>("control.mode") == "attitude") ? 1 : 0;
            status.battery          = _registry->internal_get<int>("battery.level");
            status.recording        = _registry->internal_get<int>("camera.recording");
            status.camera_connected = _registry->internal_get<bool>("camera.connected");
            status.camera_battery   = _registry->internal_get<int>("camera.battery");

            _broker->send(Broker::Channel::RADIO_COMMAND, Broker::Medium::RADIO, ByteArray((uint8_t *)&status, sizeof(status)));
        }
    }
}