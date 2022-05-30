#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

#include <platform.hpp>

#include <periph/i2c.hpp>

#include <drv/ADS1115.hpp>
#include <drv/SI4432.hpp>

#include <hal/battery.hpp>
#include <hal/button.hpp>
#include <hal/stick.hpp>
#include <hal/radio.hpp>

#define RADIO_COMMAND_CHANNEL   2

typedef struct
{
    uint8_t armed;
    float   roll;
    float   pitch;
    float   yaw;
    float   throttle;
} __attribute__((packed)) RadioCommandPayload;

int main(int argc, char ** argv)
{
    RadioCommandPayload payload;
    int                 loop = 0;
    bool                armed = false;
    bool                arming = false;

    I2C     i2c;
    Si4432  si4432 = Si4432();
    Radio   radio  = Radio(&si4432, 31);

    i2c.open(PLATFORM_BATTERY_I2C);

    ADS1115 ads = ADS1115(&i2c);

    Battery battery = Battery(&ads, 0);

    Button  button_top_left     = Button(PLATFORM_BUTTON_TOP_LEFT);
    Button  button_bottom_left  = Button(PLATFORM_BUTTON_BOTTOM_LEFT);
    Button  button_bottom_right = Button(PLATFORM_BUTTON_BOTTOM_RIGHT);
    Button  button_top_right    = Button(PLATFORM_BUTTON_TOP_RIGHT);

    Stick   stick_yaw      = Stick(PLATFORM_YAW_STICK_ADC_CHANNEL,      9638, 4200, 1.0, 1.0, 50.0, 140.0);
    Stick   stick_throttle = Stick(PLATFORM_THROTTLE_STICK_ADC_CHANNEL, 8902, 4096, 1.0, 1.2, 50.0, 140.0);
    Stick   stick_roll     = Stick(PLATFORM_ROLL_STICK_ADC_CHANNEL,     7585, 3924, 1.0, 0.2, 50.0, 140.0);
    Stick   stick_pitch    = Stick(PLATFORM_PITCH_STICK_ADC_CHANNEL,    9711, 3535, 1.0, 0.2, 50.0, 140.0);

    while (1)
    {
        usleep(1000 * 100);

        if (button_top_left.is_pressed() && button_top_right.is_pressed())
        {
            if (arming == false)
            {
                arming = true;
                armed = !armed;
            }
        }
        else
        {
            arming = false;
        }

        payload.armed     = armed;
        payload.roll      = stick_roll.get_command();
        payload.pitch     = stick_pitch.get_command();
        payload.yaw       = stick_yaw.get_command();
        payload.throttle  = stick_throttle.get_command();

        radio.send(RADIO_COMMAND_CHANNEL, ByteArray((const uint8_t *)&payload, (int)sizeof(payload)));

        if (button_bottom_left.is_pressed())
        {
            std::cout << "button left" << std::endl;
        }

        if (button_bottom_right.is_pressed())
        {
            std::cout << "button right" << std::endl;
        }

        if (++loop == 100)
        {
            std::cout << "battery : " << battery.get_level_percent() << "%" << std::endl;
        }
    }

    return 0;
}
