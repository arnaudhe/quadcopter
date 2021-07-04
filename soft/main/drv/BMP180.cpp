#include <drv/BMP180.h>
#include <drv/BMP180_defs.h>
#include <drv/BMP180_conf.h>
#include <math.h>
#include <hal/log.h>

BMP180::BMP180(I2cMaster * i2c)
{
    _i2c = i2c;
}

esp_err_t BMP180::_read_int(uint8_t reg, int16_t * value)
{
    return _read_uint(reg, (uint16_t *)value);
}

esp_err_t BMP180::_read_uint(uint8_t reg, uint16_t * value)
{
    uint8_t   buffer[2];
    esp_err_t ret;

    ret = _i2c->read_registers(BMP180_I2C_ADDR, reg, buffer, 2);

    if (ret == ESP_OK)
    {
        *value = (buffer[0] << 8) + buffer[1];
    }

    return ret;
}

esp_err_t BMP180::init()
{
    struct bmp180_calib_param_t calib; 
    double    c3, c4, b1;
    esp_err_t ret = ESP_OK;
    uint8_t   b;

    ret = _i2c->read_register(BMP180_I2C_ADDR, BMP180_CHIP_ID_REG, &b);

    if ((ret != ESP_OK) || (b != 0x55))
    {
        LOG_ERROR("Init chip id failed");
        return ESP_FAIL;
    }

    /* Retrieve calibration data from device */
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC1_MSB, &calib.ac1);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC2_MSB, &calib.ac2);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC3_MSB, &calib.ac3);
    ret |= this->_read_uint(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC4_MSB, &calib.ac4);
    ret |= this->_read_uint(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC5_MSB, &calib.ac5); 
    ret |= this->_read_uint(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_AC6_MSB, &calib.ac6);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_B1_MSB,  &calib.b1);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_B2_MSB,  &calib.b2);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_MB_MSB,  &calib.mb);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_MC_MSB,  &calib.mc);
    ret |=  this->_read_int(BMP180_PROM_START__ADDR + BMP180_CALIB_PARAM_MD_MSB,  &calib.md);

    if (ret == ESP_OK)
    {
        /* Compute doubleing-point polynominals */
        c3 = 160.0 * pow(2,-15) * calib.ac3;
        c4 = pow(10,-3) * pow(2,-15) * calib.ac4;
        b1 = pow(160,2) * pow(2,-30) * calib.b1;
        c5 = (pow(2,-15) / 160) * calib.ac5;
        c6 = calib.ac6;
        mc = (pow(2,11) / pow(160,2)) * calib.mc;
        md = calib.md / 160.0;
        x0 = calib.ac1;
        x1 = 160.0 * pow(2,-13) * calib.ac2;
        x2 = pow(160,2) * pow(2,-25) * calib.b2;
        y0 = c4 * pow(2,15);
        y1 = c4 * c3;
        y2 = c4 * b1;
        p0 = (3791.0 - 8.0) / 1600.0;
        p1 = 1.0 - 7357.0 * pow(2,-20);
        p2 = 3038.0 * 100.0 * pow(2,-36);

        LOG_INFO("Init done");
    }
    else
    {
        LOG_ERROR("Init failed");
    }

    return ret;
}

void BMP180::start_temperature(void)
{
    esp_err_t ret;

    ret = _i2c->write_register(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, BMP180_T_MEASURE);

    if (ESP_OK != ret)
    {
        LOG_ERROR("Failed to start temperature (%s)", esp_err_to_name(ret));
    }
}

void BMP180::start_pressure(void)
{
    esp_err_t ret;

    ret = _i2c->write_register(BMP180_I2C_ADDR, BMP180_CTRL_MEAS_REG, BMP180_P_MEASURE | (BMP180_CONF_OVERSAMPLING << 6));

    if (ESP_OK != ret)
    {
        LOG_ERROR("Failed to start pressure (%s)", esp_err_to_name(ret));
    }
}

esp_err_t BMP180::get_temperature(double &T)
{
    esp_err_t     ret = ESP_OK;
    uint8_t       buffer[2];
    uint16_t      raw;
    double        tu, a;
    
    ret = _i2c->read_registers(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, buffer, 2);

    raw = (buffer[0] << 8) + buffer[1];

    if (ret == ESP_OK) // good read, calculate temperature
    {
        tu = (double)raw;
        a  = c5 * (tu - c6);
        T = a + (mc / (a + md));
    }
    else
    {
        LOG_ERROR("Failed to read temperature (%s)", esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t BMP180::get_pressure(double &P, double temperature)
{
    uint8_t       data[BMP180_PRESSURE_DATA_BYTES];
    esp_err_t     ret;
    double        pu,s,x,y,z;

    ret = _i2c->read_registers(BMP180_I2C_ADDR, BMP180_ADC_OUT_MSB_REG, data, 3);

    if (ret == ESP_OK) // good read, calculate pressure
    {
        pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);

        s = temperature - 25.0;
        x = (x2 * pow(s,2)) + (x1 * s) + x0;
        y = (y2 * pow(s,2)) + (y1 * s) + y0;
        z = (pu - x) / y;
        P = (p2 * pow(z,2)) + (p1 * z) + p0;
    }
    else
    {
        LOG_ERROR("Failed to read pressure (%s)", esp_err_to_name(ret));
    }

    return ret;
}

double BMP180::sea_level(double P, double A)
{
    return (P / pow(1 - (A / 44330.0), 5.255));
}

double BMP180::altitude(double P, double P0)
{
    return (44330.0 * (1.0 - pow(P / P0, 1.0 / 5.255)));
}
