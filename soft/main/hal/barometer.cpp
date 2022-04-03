#include <hal/barometer.h>
#include <hal/log.h>
#include <math.h>
#include <platform.h>

Barometer::Barometer(I2cMaster * i2c) :
    Task("barometer", Task::Priority::MEDIUM, 4096, false),
    _i2c(i2c),
    _mutex()
{
    _bmp = new PLATFORM_BAROMETER_MODEL(i2c);
}

void Barometer::init(void)
{
    int i;
    double temperature, pressure;

    _bmp->init();

    Task::delay_ms(250);

    _initial_pressure = 0.0;

    for (i = 0; i < 20; i++)
    {
        _bmp->read_temperature_pressure(&temperature, &pressure);
        _initial_pressure += pressure;
    }

    _initial_pressure = _initial_pressure / 20.0;
    _temperature = temperature;

    LOG_INFO("Initial pressure %f", _initial_pressure);
    LOG_INFO("Initial temperature %f", _temperature);

    start();
}

double Barometer::sea_level(double P, double A)
{
    return (P / pow(1 - (A / 44330.0), 5.255));
}

double Barometer::altitude(double P, double P0)
{
    return (44330.0 * (1.0 - pow(P / P0, 1.0 / 5.255)));
}

void Barometer::run()
{
    double pressure;

    while(1)
    {
        _bmp->read_temperature_pressure(&_temperature, &pressure);
        _mutex.lock();
        _height = this->altitude(pressure, _initial_pressure);
        _mutex.unlock();
    }
}

float Barometer::height()
{
    float tmp;
    _mutex.lock();
    tmp = (float)_height;
    _mutex.unlock();
    return tmp;
}