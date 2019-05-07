#include <hal/barometer.h>
#include <esp_log.h>

Barometer::Barometer(I2cMaster * i2c) : 
    Task("barometer", Task::Priority::VERY_LOW, 2048, false),
    _i2c(i2c),
    _mutex()
{
    _bmp = new BMP180(i2c);
}

void Barometer::init(void)
{
    int i;

    _bmp->init();

    Task::delay_ms(250);

    _initial_pressure = 0.0;

    for (i = 0; i < 20; i++)
    {
        double tmp;

        _bmp->start_temperature();
        Task::delay_ms(5);
        _bmp->get_temperature(_temperature);

        _bmp->start_pressure();
        Task::delay_ms(26);
        _bmp->get_pressure(tmp, _temperature);

        _initial_pressure += tmp;
    }

    _initial_pressure = _initial_pressure / 20.0;

    ESP_LOGI("Barometer", "initial pressure %f", _initial_pressure);
    ESP_LOGI("Barometer", "initial temperature %f", _temperature);

    start();
}

void Barometer::run()
{
    double pressure;

    while(1)
    {
        _bmp->start_temperature();
        Task::delay_ms(5);
        _bmp->get_temperature(_temperature);

        _bmp->start_pressure();
        Task::delay_ms(26);
        _bmp->get_pressure(pressure, _temperature);

        _mutex.lock();
        _height = _bmp->altitude(pressure, _initial_pressure);
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