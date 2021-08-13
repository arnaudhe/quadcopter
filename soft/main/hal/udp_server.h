#pragma once

#include <string>
#include <functional>

#include <os/task.h>
#include <os/semaphore.h>

using namespace std;

class UdpServer : public Task
{
  private:

    string                              _name;
    int                                 _port;
    function<void(string, string&)>     _callback;
    int                                 _socket;

    void run();

    bool recv(string &msg);

    bool recvfrom(string &msg, string &address, int &port);

    bool sendto(string msg, string address, int port);

  public:

    UdpServer(string name, int port);
    ~UdpServer();

    void register_callback(function<void(string, string&)>);

    bool send_broadcast(string msg, int port);

};