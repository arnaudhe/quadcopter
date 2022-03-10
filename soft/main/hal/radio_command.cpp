#include <hal/radio_command.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <string.h>

#include <hal/log.h>

#include <platform.h>

#define RADIO_COMMAND_UDP_PORT      PLATFORM_UDP_PORT_BASE + RADIO_COMMAND_CHANNEL

#define RADIO_COMMAND_FAILSAFE      1500

typedef struct
{
    uint8_t armed;
    float   roll;
    float   pitch;
    float   yaw;
    float   throttle;
} __attribute__((packed)) RadioCommandPacket;

RadioCommand::RadioCommand(DataRessourcesRegistry * registry, RadioBroker * radio) :
    Task("rc", Task::Priority::LOW, 4096, false)
{
    _registry = registry;
    _radio = radio;

    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    destAddr.sin_family      = AF_INET;
    destAddr.sin_port        = htons(RADIO_COMMAND_UDP_PORT);

    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (_socket < 0) 
    {
        LOG_ERROR("Unable to create socket: errno %d", errno);
        return;
    }

    LOG_INFO("Socket created");

    int err = ::bind(_socket, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (err < 0)
    {
        LOG_ERROR("Socket unable to bind: errno %d", errno);
    }

    int flags = fcntl(_socket, F_GETFL);
    fcntl(_socket, F_SETFL, flags | O_NONBLOCK);

    _radio->register_channel(RADIO_COMMAND_CHANNEL);

    LOG_INFO("Radio channel registered");

    LOG_INFO("Radio command ready");
}

RadioCommand::~RadioCommand()
{
    close(_socket);
}

void RadioCommand::run()
{
    RadioCommandPacket packet;
    uint8_t            len;
    TickType_t         last_command_tick = 0;

    while (1)
    {
        if (_radio->received_frame_pending(RADIO_COMMAND_CHANNEL))
        {
            /* Get the received radio frame */
            _radio->receive(RADIO_COMMAND_CHANNEL, (uint8_t *)&packet, &len);
        }
        else
        {
            /* Get any pending UDP received frame */
            len = (uint8_t)::recv(_socket, &packet, sizeof(RadioCommandPacket), O_NONBLOCK);
        }

        if (len == sizeof(RadioCommandPacket))
        {
            last_command_tick = xTaskGetTickCount();

            _registry->internal_set<string>("control.mode", packet.armed ? "attitude" : "off");
            _registry->internal_set<float>("control.attitude.roll.position.target",  packet.roll);
            _registry->internal_set<float>("control.attitude.pitch.position.target", packet.pitch);
            _registry->internal_set<float>("control.attitude.height.speed.target", packet.throttle);
            _registry->internal_set<float>("control.attitude.yaw.speed.target", packet.yaw);
        }
        else if ((last_command_tick > 0) && ((TickType_t)(xTaskGetTickCount() - last_command_tick) > RADIO_COMMAND_FAILSAFE))
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