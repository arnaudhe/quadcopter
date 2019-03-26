#include <hal/barometer.h>

Barometer::Barometer(I2cMaster * i2c)
{
    _bmp = new BMP180(i2c);
    _bmp->init();
}

void Barometer::read(double &baro)
{
    _bmp->get_pressure(baro);
}