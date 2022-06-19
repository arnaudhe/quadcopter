#pragma once

#include <os/task.hpp>

#include <drv/SI4432.hpp>

#include <hal/radio.hpp>
#include <hal/button.hpp>
#include <hal/stick.hpp>

class RadioCommand : public Task
{

private:

    struct  __attribute__((packed)) Payload
    {
        uint8_t armed;
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

    void run(void);

public:

    RadioCommand(void);

    int link_quality(void);

    int quadcopter_battery(void);

};