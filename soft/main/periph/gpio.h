#pragma once

#include <driver/gpio.h>
#include <soc/gpio_reg.h>
#include <functional>

using namespace std;

class Gpio
{

  public:

    enum Direction
    {
        INPUT,
        OUTPUT
    };

    enum InterruptSource
    {
        RISING_EDGE,
        FALLING_EDGE,
        BOTH_EDGE
    };

    Gpio(int pin, Gpio::Direction direction, bool pull_up, bool pull_down);

    bool read();

    void write(bool);

    void enable_interrupt(Gpio::InterruptSource source, std::function<void()>);

  private:

    gpio_num_t              _pin;
    Gpio::Direction         _direction;
    std::function<void()>   _callback;

    static void isr_handler(void * arg);
};