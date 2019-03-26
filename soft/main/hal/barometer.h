#pragma once

#include <periph/i2c_master.h>
#include <drv/BMP180.h>

class Barometer
{
private:

    BMP180 * _bmp;

public:

    Barometer(I2cMaster * i2c);

    void read(double &baro);

};