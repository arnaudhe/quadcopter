#pragma once

#include <string>
#include <functional>
#include <tuple>
#include <utils/byte_array.h>

using namespace std;

class Udp
{
  private:

    int _socket;
    int _port;

  public:

    Udp(int port);
    ~Udp();

    ByteArray receive(int size);

    tuple<ByteArray, string, int> receive_from(int size);

    bool send_to(ByteArray data, string address, int port);

    bool send_broadcast(ByteArray data, int port);

};