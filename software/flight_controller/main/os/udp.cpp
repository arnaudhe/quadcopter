#include <os/udp.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <string.h>

#include <hal/log.h>

#include <alloca.h>

Udp::Udp(int port)
{
    int broadcast = 1;
    struct sockaddr_in sockaddr;

    _port = port;

    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons(_port);

    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (_socket < 0)
    {
        LOG_ERROR("Unable to create socket: errno %d", errno);
        return;
    }

    LOG_INFO("Socket created");

    int err = ::bind(_socket, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (err < 0)
    {
        LOG_ERROR("Socket unable to bind: errno %d", errno);
    }

    if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast, sizeof(broadcast)) < 0)
    {
        LOG_ERROR("Failed to set broadcast option");
        closesocket(_socket);
    }

    int flags = fcntl(_socket, F_GETFL);
    fcntl(_socket, F_SETFL, flags | O_NONBLOCK);

    LOG_INFO("Socket ready");
}

Udp::~Udp()
{
    close(_socket);
}

ByteArray Udp::receive(int size)
{
    uint8_t * recv_buffer = (uint8_t * )alloca(size);
    int       recv_len;

    recv_len = (int)::recv(_socket, (void *)recv_buffer, size, O_NONBLOCK);
    return ByteArray(recv_buffer, recv_len);
}

tuple<ByteArray, string, int> Udp::receive_from(int size)
{
    struct sockaddr_in  source_addr;
    socklen_t           source_addr_len = sizeof(source_addr);
    char                ip_address[INET_ADDRSTRLEN];
    int                 recv_len;
    uint8_t           * recv_buffer = (uint8_t *)alloca(size);

    recv_len = (int)::recvfrom(_socket, (void *)recv_buffer, size, O_NONBLOCK, (struct sockaddr *)&source_addr, &source_addr_len);

    if (recv_len > 0)
    {
        if (inet_ntop(source_addr.sin_family, &(source_addr.sin_addr), ip_address, INET_ADDRSTRLEN) != NULL)
        {
            int port = ntohs(source_addr.sin_port);
            return tuple<ByteArray, string, int>{ByteArray(recv_buffer, recv_len), string(ip_address), port};
        }
    }

    return std::tuple<ByteArray, string, int>{ByteArray(), "", 0};
}

bool Udp::send_to(ByteArray data, string address, int port)
{
    struct sockaddr_in dest_addr;
    int                sent_len;

    inet_aton(address.c_str(), &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons(port);

    sent_len = (int)::sendto(_socket, data.data(), data.length(), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent_len > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Udp::send_broadcast(ByteArray data, int port)
{
    struct sockaddr_in dest_addr;
    int                sent_len;

    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_addr.s_addr = IPADDR_BROADCAST;
    dest_addr.sin_port        = htons(port);

    sent_len = (int)::sendto(_socket, data.data(), data.length(), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    if (sent_len > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
