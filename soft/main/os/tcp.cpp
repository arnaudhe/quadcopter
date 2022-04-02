#include <os/tcp.h>
#include <hal/log.h>

#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <sys/param.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

Tcp::Tcp()
{
    _connected = false;
}

Tcp::~Tcp()
{
    close(_socket);
}

bool Tcp::connect(string address, int port)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(address.c_str());
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    if (_connected == false)
    {
        _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (_socket < 0)
        {
            LOG_ERROR("Unable to create socket: errno %d", errno);
        }

        int err = ::connect(_socket, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err == 0)
        {
            _connected = true;
        }
        else
        {
            ::close(_socket);
            LOG_ERROR("Socket unable to connect: errno %d", errno);
        }
    }
    else
    {
        LOG_WARNING("Already connected");
    }

    return _connected;
}

void Tcp::disconnect()
{
    ::close(_socket);
    _connected = false;
}

bool Tcp::is_connected()
{
    return _connected;
}

bool Tcp::send(ByteArray data)
{
    if (_connected == true)
    {
        if (::send(_socket, data.data(), data.length(), 0) == data.length())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        LOG_WARNING("Not connected");
        return false;
    }
}

ByteArray Tcp::receive(int size)
{
    uint8_t * recv_buffer = (uint8_t * )alloca(size);
    int       recv_len;

    if (_connected == true)
    {
        recv_len = ::recv(_socket, recv_buffer, size, 0);
        if (recv_len == -1)
        {
            disconnect();
            LOG_INFO("Connection lost %d", errno);
            return ByteArray();
        }
        else
        {
            return ByteArray(recv_buffer, recv_len);
        }
    }
    else
    {
        LOG_WARNING("Not connected");
        return ByteArray();
    }
}
