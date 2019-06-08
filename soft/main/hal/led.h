#pragma once

#include <periph/gpio.h>

class Led
{

  private:

    Gpio * _gpio;
    bool   _state;

  public:

    Led();

    void on(void);
    void off(void);
    void toggle(void);

};
