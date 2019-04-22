#pragma once

#include <periph/gpio.h>
#include <periph/pulse.h>
#include <periph/timer.h>

class HcSr04
{

  private:

    Gpio  * _echo_gpio;
    Pulse * _trig_pulse;
    Timer * _echo_timer;

    float _height;

    void echo_handler(void);

  public:

    HcSr04(int echo_pin, int trig_channel, int trig_pin);

    float height();
};