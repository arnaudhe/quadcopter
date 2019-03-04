#pragma once

#include <periph/i2c_master.h>

class Barometer
{
private:

    I2cMaster * _i2c;

public:

    Barometer(I2cMaster * i2c);

    void read(float * baro);

};