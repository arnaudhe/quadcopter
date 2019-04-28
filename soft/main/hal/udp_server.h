#pragma once

#include <string>
#include <functional>

using namespace std;

class UdpServer
{
  private:

    string                              _name;
    int                                 _port;
    function<void(string, string&)>     _callback;
    int                                 _socket;

    static void udp_server_task(void *);

    bool recv(string &msg);

    bool recvfrom(string &msg, string &address, int &port);

    bool sendto(string msg, string address, int port);

  public:

    UdpServer(string name, int port);
    ~UdpServer();

    void register_callback(function<void(string, string&)>);

    void start(void);

};