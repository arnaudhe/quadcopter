#include <hal/radio_command.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <string.h>

#include <hal/log.h>

#define RADIO_COMMAND_UDP_PORT      5005

typedef struct
{
    uint8_t armed;
    float   roll;
    float   pitch;
    float   yaw;
    float   throttle;
} __attribute__((packed)) RadioCommandPacket;

RadioCommand::RadioCommand(DataRessourcesRegistry * registry) :
    Task("rc", Task::Priority::LOW, 4096, false)
{
    _registry = registry;

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

    LOG_INFO("Radio command ready");
}

RadioCommand::~RadioCommand()
{
    close(_socket);
}

void RadioCommand::run()
{
    RadioCommandPacket packet;
    int                len;

    while (1)
    {
        len = ::recv(_socket, &packet, sizeof(RadioCommandPacket), 0);

        if (len == sizeof(RadioCommandPacket))
        {
            _registry->internal_set<string>("control.mode", packet.armed ? "attitude" : "off");
            _registry->internal_set<float>("control.attitude.roll.position.target",  packet.roll);
            _registry->internal_set<float>("control.attitude.pitch.position.target", packet.pitch);
            _registry->internal_set<float>("control.attitude.height.manual.throttle", packet.throttle);
            _registry->internal_set<float>("control.attitude.yaw.speed.target", packet.yaw);
        }
    }
}