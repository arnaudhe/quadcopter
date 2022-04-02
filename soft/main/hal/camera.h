#pragma once

#include <os/tcp.h>
#include <os/task.h>
#include <os/queue.h>
#include <os/mutex.h>
#include <hal/wifi.h>
#include <string>
#include <utils/byte_array.h>

using namespace std;

class Camera : public Task
{

  private:

    enum Request
    {
        NONE,
        TAKE_PICTURE,
        START_RECORDING,
        STOP_RECORDING
    };

    enum Command
    {
        COMMAND_GET_BATTERY     = 13,
        COMMAND_AUTHENT         = 257,
        COMMAND_START_RECORDING = 513,
        COMMAND_STOP_RECORDING  = 514,
        COMMAND_PHOTO           = 770,
    };

    Tcp              * _tcp;
    Wifi             * _wifi;
    int                _battery_level;
    Request            _request;
    int                _token;
    ByteArray        * _recv_data;
    Mutex            * _mutex;

    void run(void);

    bool send_request(Command command);
    bool wait_response(Command &command, int &rval, int &param);

    bool execute_command(Command command, int &output_param);
    bool execute_command(Command command);

    void get_token();

  public:

    Camera(Wifi * wifi);

    bool is_connected();

    int get_battery_level();

    bool take_picture();

    bool start_recording();

    bool stop_recording();
};
