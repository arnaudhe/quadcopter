#pragma once

#include <string>
#include <utils/byte_array.h>

using namespace std;

class Tcp
{

  private:

    int _socket;
    int _connected;

  public:

    Tcp();
    ~Tcp();

    bool connect(string address, int port);

    void disconnect();

    bool is_connected();

    bool send(ByteArray data);

    ByteArray receive(int size);

};
