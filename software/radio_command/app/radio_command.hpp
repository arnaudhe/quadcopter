#pragma once

#include <os/task.hpp>

#include <drv/SI4432.hpp>

#include <hal/radio.hpp>
#include <hal/button.hpp>
#include <hal/stick.hpp>

class RadioCommand : public Task
{

private:

    struct __attribute__((packed)) CommandsPayload
    {
        uint8_t armed;
        uint8_t record;
        uint8_t camera_tilt;
        float   roll;
        float   pitch;
        float   yaw;
        float   throttle;
    };

    struct __attribute__((packed)) StatusPayload
    {
        uint8_t armed;
        uint8_t battery;
        uint8_t link_quality;
        uint8_t recording;
        uint8_t camera_connected;
        uint8_t camera_battery;
    };

    Button _button_top_left;
    Button _button_bottom_left;
    Button _button_bottom_right;
    Button _button_top_right;

    Stick  _stick_yaw;
    Stick  _stick_throttle;
    Stick  _stick_roll;
    Stick  _stick_pitch;

    Si4432  _si4432;
    Radio   _radio;

    bool  _lost;
    bool  _armed;
    int   _link_quality;
    int   _quadcopter_battery;
    int   _camera_battery;
    bool  _camera_connected;
    bool  _camera_recording;

    void run(void);

public:

    RadioCommand(void);

    void start(void);

    bool lost(void);

    bool armed(void);

    int link_quality(void);

    int quadcopter_battery(void);

    int camera_battery(void);

    bool camera_connected(void);

    bool camera_recording(void);

};