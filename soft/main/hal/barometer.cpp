#include <hal/barometer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

Barometer::Barometer(I2cMaster * i2c)
{
    _bmp = new BMP180(i2c);
}

void Barometer::init(void)
{
    int i;

    _bmp->init();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    _initial_pressure = 0.0;

    for (i = 0; i < 20; i++)
    {
        double tmp;

        _bmp->start_temperature();
        vTaskDelay(5 / portTICK_PERIOD_MS);
        _bmp->get_temperature(_temperature);

        _bmp->start_pressure();
        vTaskDelay(26 / portTICK_PERIOD_MS);
        _bmp->get_pressure(tmp, _temperature);

        _initial_pressure += tmp;
    }

    _initial_pressure = _initial_pressure / 20.0;

    ESP_LOGI("Barometer", "initial pressure %f", _initial_pressure);
    ESP_LOGI("Barometer", "initial temperature %f", _temperature);
}

void Barometer::read_temperature(double &temperature)
{
    _bmp->get_temperature(_temperature);
    temperature = _temperature;
    _bmp->start_pressure();
}

void Barometer::read(double &baro)
{
    double pressure;
    _bmp->get_pressure(pressure, _temperature);
    baro = _bmp->altitude(pressure, _initial_pressure);
    _bmp->start_temperature();
}