#pragma once

#include <periph/gpio.hpp>

class Button
{

private:

    Gpio _gpio;

public:

    Button(int pin);

    bool is_pressed(void);

};
