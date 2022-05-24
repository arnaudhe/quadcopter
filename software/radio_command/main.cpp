#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <platform.hpp>

#include <periph/i2c.hpp>

#include <drv/ADS1115.hpp>
#include <drv/SI4432.hpp>

#include <hal/battery.hpp>
#include <hal/button.hpp>
#include <hal/stick.hpp>

int main(int argc, char ** argv)
{
    int rssi;
    I2C i2c;

    i2c.open(PLATFORM_BATTERY_I2C);

    ADS1115 ads = ADS1115(&i2c);

    Battery battery = Battery(&ads, 0);

    Button  button_top_left     = Button(PLATFORM_BUTTON_TOP_LEFT);
    Button  button_bottom_left  = Button(PLATFORM_BUTTON_BOTTOM_LEFT);
    Button  button_bottom_right = Button(PLATFORM_BUTTON_BOTTOM_RIGHT);
    Button  button_top_right    = Button(PLATFORM_BUTTON_TOP_RIGHT);

    Si4432  si4432 = Si4432();

    Stick   stick_yaw      = Stick(PLATFORM_YAW_STICK_ADC_CHANNEL,      9638, 4200, 1.0, 1.0, 50.0, 140.0);
    Stick   stick_throttle = Stick(PLATFORM_THROTTLE_STICK_ADC_CHANNEL, 8902, 4096, 1.0, 1.2, 50.0, 140.0);
    Stick   stick_roll     = Stick(PLATFORM_ROLL_STICK_ADC_CHANNEL,     7585, 3924, 1.0, 0.2, 50.0, 140.0);
    Stick   stick_pitch    = Stick(PLATFORM_PITCH_STICK_ADC_CHANNEL,    9711, 3535, 1.0, 0.2, 50.0, 140.0);

    for (int i = 0; i < 10; i++)
    {
        usleep(1000 * 100);

        printf("            battery : %f\n\n", battery.get_voltage());

        printf("    button_top_left : %d\n", button_top_left.is_pressed());
        printf(" button_bottom_left : %d\n", button_bottom_left.is_pressed());
        printf("button_bottom_right : %d\n", button_bottom_right.is_pressed());
        printf("   button_top_right : %d\n\n", button_top_right.is_pressed());

        printf("     stick_throttle : %f\n", stick_throttle.get_command());
        printf("          stick_yaw : %f\n", stick_yaw.get_command());
        printf("         stick_roll : %f\n", stick_roll.get_command());
        printf("        stick_pitch : %f\n\n", stick_pitch.get_command());

        si4432.read_rssi(&rssi);

        printf("        si4432 rssi : %d\n\n", rssi);
    }

    return 0;
}
