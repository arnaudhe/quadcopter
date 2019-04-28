#pragma once

#include <string>
#include <esp_err.h>
#include <esp_event_loop.h>

using namespace std;

class Wifi
{

  private:

    typedef enum
    {
        OFF,
        STARTED,
        CONNECTED
    }State;

    State _state;
    bool  _connection_request;
    bool  _disconnection_request;

    static esp_err_t event_handler(void *ctx, system_event_t *event);

  public:

    Wifi();

    void connect();

    void disconnect();
};