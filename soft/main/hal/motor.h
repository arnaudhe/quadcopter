#pragma once

#include <drv/DShotRMT.h>

class Motor
{

private:

    DShotRMT * _dshot;

public:

    Motor(int channel, int pin);

    void arm(void);

    void set_speed(float speed);

    void set_zero(void);

};