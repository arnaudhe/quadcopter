#pragma once

#include <periph/i2c_master.h>
#include <drv/BMP180.h>

class Barometer
{
private:

    double   _temperature;
    double   _initial_pressure;
    BMP180 * _bmp;

public:

    Barometer(I2cMaster * i2c);

    void init(void);

    void read_temperature(double &temperature);

    void read(double &baro);

};