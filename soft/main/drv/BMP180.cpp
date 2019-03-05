#include <drv/BMP180.h>

BMP180::BMP180(I2cMaster * i2c)
{
    _i2c = i2c;
}

esp_err_t BMP180::init(void)
{
    return ESP_OK;
}

esp_err_t BMP180::read(float * baro)
{
    return ESP_OK;
}