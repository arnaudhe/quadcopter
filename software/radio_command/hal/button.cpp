#include "button.hpp"

Button::Button(int pin) : 
    _gpio(pin, Gpio::OUTPUT)
{
}

bool Button::is_pressed(void)
{
    /* Gpio is pulled to GND when released, connected to VCC when pressed */
    return _gpio.read();
}
