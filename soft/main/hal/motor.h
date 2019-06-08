#pragma once

#include <periph/pulse.h>

class Motor
{

private:

    Pulse * _pulse;

public:

    Motor(int channel, int pin);

    void set_speed(float speed);

    void set_zero(void);

};