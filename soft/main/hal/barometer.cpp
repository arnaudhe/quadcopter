#include <hal/barometer.h>

Barometer::Barometer(I2cMaster * i2c)
{
    _bmp = new BMP180(i2c);
    _bmp->init();
}

void Barometer::read(float * baro)
{
    _bmp->read(baro);
}