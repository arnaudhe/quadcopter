#include "radio_command.hpp"

#include <platform.hpp>

#include <iostream>

#define RADIO_COMMAND_CHANNEL   2
#define RADIO_COMMAND_ADDRESS   31

RadioCommand::RadioCommand(void):
    Task("rc", false),
    _button_top_left(PLATFORM_BUTTON_TOP_LEFT),
    _button_bottom_left(PLATFORM_BUTTON_BOTTOM_LEFT),
    _button_bottom_right(PLATFORM_BUTTON_BOTTOM_RIGHT),
    _button_top_right(PLATFORM_BUTTON_TOP_RIGHT),
    _stick_yaw(PLATFORM_YAW_STICK_ADC_CHANNEL, 9638, 4200, 1.0, 1.0, 50.0, 140.0),
    _stick_throttle(PLATFORM_THROTTLE_STICK_ADC_CHANNEL, 8902, 4096, 1.0, 1.2, 50.0, 140.0),
    _stick_roll(PLATFORM_ROLL_STICK_ADC_CHANNEL, 7585, 3924, 1.0, 0.2, 50.0, 140.0),
    _stick_pitch(PLATFORM_PITCH_STICK_ADC_CHANNEL, 9711, 3535, 1.0, 0.2, 50.0, 140.0),
    _si4432(),
    _radio(&_si4432, RADIO_COMMAND_ADDRESS)
{
    return;
}

void RadioCommand::run(void)
{
    Payload payload;    
    bool    armed = false;
    bool    arming = false;

    while (running())
    {
        if (_button_top_left.is_pressed() && _button_top_right.is_pressed())
        {
            if (arming == false)
            {
                arming = true;
                armed = !armed;
                if (armed)
                {
                    std::cout << "ARMED !" << std::endl;
                }
                else
                {
                    std::cout << "DISARMED !" << std::endl;
                }
            }
        }
        else
        {
            arming = false;
        }

        payload.armed     = armed;
        payload.roll      = _stick_roll.get_command();
        payload.pitch     = _stick_pitch.get_command();
        payload.yaw       = _stick_yaw.get_command();
        payload.throttle  = _stick_throttle.get_command();

        _radio.send(RADIO_COMMAND_CHANNEL, ByteArray((const uint8_t *)&payload, (int)sizeof(payload)));

        Task::delay_ms(50);
    }
}