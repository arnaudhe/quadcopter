#pragma once

#include <string>

using namespace std;

class UdpServer
{
  private:

    string  _name;
    int     _port;

    static void udp_server_task(void *);

  public:

    UdpServer(string name, int port);
};