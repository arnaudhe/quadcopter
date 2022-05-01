#include <hal/led.h>

Led::Led(void)
{
    _gpio  = new Gpio(16, Gpio::Direction::OUTPUT, false, false);
    _state = false;
    _gpio->write(false);
}

void Led::on(void)
{
    _state = true;
    _gpio->write(true);
}

void Led::off(void)
{
    _state = false;
    _gpio->write(false);
}

void Led::toggle(void)
{
    _state = !_state;
    _gpio->write(_state);
}