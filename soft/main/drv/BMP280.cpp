#include <drv/BMP280.h>
#include <drv/BMP280_conf.h>
#include <hal/log.h>
#include <os/task.h>

BMP280::BMP280(I2cMaster * i2c)
{
    _i2c = i2c;
}

esp_err_t BMP280::init()
{
    esp_err_t ret;

    /* Maximum number of tries before timeout */
    uint8_t try_count;

    this->_chip_id = 0;

    for (try_count = 0; try_count < 5; try_count++)
    {
        ret = _i2c->read_register(BMP280_I2C_ADDR_PRIM, BMP280_CHIP_ID_ADDR, &this->_chip_id);

        LOG_INFO("chip id %02x", this->_chip_id);

        /* Check for chip id validity */
        if ((ret == ESP_OK) && (this->_chip_id == BMP280_CHIP_ID1 || this->_chip_id == BMP280_CHIP_ID2 ||
                                this->_chip_id == BMP280_CHIP_ID3 || this->_chip_id == BMP280_CHIP_BME))
        {
            LOG_INFO("Reset the device");
            this->soft_reset();
            LOG_INFO("Read calibration");
            this->_get_calib();
            LOG_INFO("Write config");
            this->_write_config();

            return ESP_OK;
        }
        else
        {
            LOG_WARNING("Init chip id failed");
        }

        /* Wait for 10 ms before try again */
        Task::delay_ms(10);
    }

    /* Chip id check failed, and timed out */
    LOG_ERROR("Init failed after %d retries", try_count);
    return ESP_FAIL;
}

esp_err_t BMP280::_write_config(void)
{
    esp_err_t ret;
    uint8_t   buffer[2] = { 0, 0 };

    ret = _i2c->read_registers(BMP280_I2C_ADDR_PRIM, BMP280_CTRL_MEAS_ADDR, buffer, 2);
    if (ret != ESP_OK)
    {
        LOG_ERROR("Failed to read config");
        return ret;
    }

    /* Perform a reset to ensure we are in sleep mode */
    ret = this->soft_reset();
    if (ret != ESP_OK)
    {
        LOG_ERROR("Failed to reset device");
        return ret;
    }

    buffer[0] = ((buffer[0] & ~BMP280_POWER_MODE_MASK) | ((BMP280_SLEEP_MODE << BMP280_POWER_MODE_POS) & BMP280_POWER_MODE_MASK));
    buffer[0] = ((buffer[0] & ~BMP280_OS_TEMP_MASK) | ((BMP280_OS_TEMP << BMP280_OS_TEMP_POS) & BMP280_OS_TEMP_MASK));
    buffer[0] = ((buffer[0] & ~BMP280_OS_PRES_MASK) | ((BMP280_OS_PRES << BMP280_OS_PRES_POS) & BMP280_OS_PRES_MASK));
    buffer[1] = ((buffer[1] & ~BMP280_STANDBY_DURN_MASK) | ((BMP280_ODR << BMP280_STANDBY_DURN_POS) & BMP280_STANDBY_DURN_MASK));
    buffer[1] = ((buffer[1] & ~BMP280_FILTER_MASK) | ((BMP280_FILTER << BMP280_FILTER_POS) & BMP280_FILTER_MASK));
    buffer[1] = ((buffer[1] & ~BMP280_SPI3_ENABLE_MASK) | ((BMP280_SPI3W_EN << BMP280_SPI3_ENABLE_POS) & BMP280_SPI3_ENABLE_MASK));

    ret = _i2c->write_register(BMP280_I2C_ADDR_PRIM, BMP280_CTRL_MEAS_ADDR, buffer[0]);
    if (ret != ESP_OK)
    {
        LOG_ERROR("Failed to write config");
        return ret;
    }

    ret = _i2c->write_register(BMP280_I2C_ADDR_PRIM, BMP280_CONFIG_ADDR, buffer[1]);
    if (ret != ESP_OK)
    {
        LOG_ERROR("Failed to write config");
        return ret;
    }

    /* Write only the power mode register in a separate write */
    buffer[0] = ((buffer[0] & ~BMP280_POWER_MODE_MASK) | ((BMP280_NORMAL_MODE << BMP280_POWER_MODE_POS) & BMP280_POWER_MODE_MASK));

    ret = _i2c->write_register(BMP280_I2C_ADDR_PRIM, BMP280_CTRL_MEAS_ADDR, buffer[0]);
    if (ret != ESP_OK)
    {
        LOG_ERROR("Failed to set power mode");
        return ret;
    }

    return ESP_OK;
}

esp_err_t BMP280::_get_calib(void)
{
    esp_err_t ret;
    uint8_t buffer[BMP280_CALIB_DATA_SIZE] = { 0 };

    ret = _i2c->read_registers(BMP280_I2C_ADDR_PRIM, BMP280_DIG_T1_LSB_ADDR, buffer, BMP280_CALIB_DATA_SIZE);

    if (ret == ESP_OK)
    {
        _calib_param.dig_t1 = (uint16_t) (((uint16_t) buffer[BMP280_DIG_T1_MSB_POS] << 8) | ((uint16_t) buffer[BMP280_DIG_T1_LSB_POS]));
        _calib_param.dig_t2 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_T2_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_T2_LSB_POS]));
        _calib_param.dig_t3 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_T3_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_T3_LSB_POS]));
        _calib_param.dig_p1 = (uint16_t) (((uint16_t) buffer[BMP280_DIG_P1_MSB_POS] << 8) | ((uint16_t) buffer[BMP280_DIG_P1_LSB_POS]));
        _calib_param.dig_p2 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P2_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P2_LSB_POS]));
        _calib_param.dig_p3 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P3_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P3_LSB_POS]));
        _calib_param.dig_p4 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P4_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P4_LSB_POS]));
        _calib_param.dig_p5 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P5_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P5_LSB_POS]));
        _calib_param.dig_p6 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P6_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P6_LSB_POS]));
        _calib_param.dig_p7 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P7_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P7_LSB_POS]));
        _calib_param.dig_p8 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P8_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P8_LSB_POS]));
        _calib_param.dig_p9 = (int16_t)  (((int16_t)  buffer[BMP280_DIG_P9_MSB_POS] << 8) | ((int16_t) buffer[BMP280_DIG_P9_LSB_POS]));
    }
    else
    {
        LOG_ERROR("Failed to read calibration");
    }

    return ret;
}

double BMP280::_get_compensated_temperature(int32_t uncomp_temperature)
{
    double var1, var2, temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    var1 = (((double) uncomp_temperature)  / 16384.0  - ((double) _calib_param.dig_t1) / 1024.0)  * ((double) _calib_param.dig_t2);
    var2 = ((((double) uncomp_temperature) / 131072.0 - ((double) _calib_param.dig_t1) / 8192.0)  *
           (((double) uncomp_temperature)  / 131072.0 - ((double) _calib_param.dig_t1) / 8192.0)) *
           ((double) _calib_param.dig_t3);
    _calib_param.t_fine = (int32_t) (var1 + var2);
    temperature = ((var1 + var2) / 5120.0);

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

double BMP280::_get_compensated_pressure(int32_t uncomp_pressure)
{
    double var1;
    double var2;
    double var3;
    double pressure;
    double pressure_min = 30000.0;
    double pressure_max = 110000.0;

    var1 = ((double)_calib_param.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)_calib_param.dig_p6) / 32768.0;
    var2 = var2 + var1 * ((double)_calib_param.dig_p5) * 2.0;
    var2 = (var2 / 4.0) + (((double)_calib_param.dig_p4) * 65536.0);
    var3 = ((double)_calib_param.dig_p3) * var1 * var1 / 524288.0;
    var1 = (var3 + ((double)_calib_param.dig_p2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)_calib_param.dig_p1);

    /* avoid exception caused by division by zero */
    if (var1 > (0.0))
    {
        pressure = 1048576.0 - (double) uncomp_pressure;
        pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double)_calib_param.dig_p9) * pressure * pressure / 2147483648.0;
        var2 = pressure * ((double)_calib_param.dig_p8) / 32768.0;
        pressure = pressure + (var1 + var2 + ((double)_calib_param.dig_p7)) / 16.0;

        if (pressure < pressure_min)
        {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max)
        {
            pressure = pressure_max;
        }
    }
    else /* Invalid case */
    {
        pressure = pressure_min;
    }

    return pressure;
}

esp_err_t BMP280::soft_reset()
{
    esp_err_t ret;
    uint8_t reg_addr = BMP280_SOFT_RESET_ADDR;
    uint8_t soft_rst_cmd = BMP280_SOFT_RESET_CMD;

    ret = _i2c->write_register(BMP280_I2C_ADDR_PRIM, reg_addr, soft_rst_cmd);

    /* As per the datasheet, startup time is 2 ms. */
    Task::delay_ms(2);

    return ret;
}

esp_err_t BMP280::get_status(bool * measuring, bool * im_update)
{
    esp_err_t ret;

    ret = _i2c->read_bit(BMP280_I2C_ADDR_PRIM, BMP280_STATUS_ADDR, BMP280_STATUS_MEAS_POS, measuring);

    if (ret == ESP_OK)
    {
        ret = _i2c->read_bit(BMP280_I2C_ADDR_PRIM, BMP280_STATUS_ADDR, BMP280_STATUS_IM_UPDATE_POS, im_update);
    }

    return ret;
}

int BMP280::get_measure_time()
{
    int period = 0;
    uint32_t t_dur = 0, p_dur = 0, p_startup = 0;
    const uint32_t startup = 1000, period_per_osrs = 2000; /* Typical timings in us. Maximum is +15% each */

    t_dur = period_per_osrs * ((1 << BMP280_OS_TEMP) >> 1);
    p_dur = period_per_osrs * ((1 << BMP280_OS_PRES) >> 1);
    p_startup = (BMP280_OS_PRES) ? 500 : 0;

    /* Increment the value to next highest integer if greater than 0.5 */
    period = startup + t_dur + p_startup + p_dur + 500;
    period /= 1000; /* Convert to milliseconds */

    return period;
}

esp_err_t BMP280::read_temperature_pressure(double * temperature, double * pressure)
{
    esp_err_t ret;
    uint8_t temp[6] = { 0 };
    int32_t uncomp_pressure;
    int32_t uncomp_temperature;

    ret = _i2c->read_registers(BMP280_I2C_ADDR_PRIM, BMP280_PRES_MSB_ADDR, temp, 6);

    if (ret == ESP_OK)
    {
        uncomp_pressure    = (int32_t) ((((uint32_t) (temp[0])) << 12) | (((uint32_t) (temp[1])) << 4) | ((uint32_t) temp[2] >> 4));
        uncomp_temperature = (int32_t) ((((int32_t) (temp[3])) << 12) | (((int32_t) (temp[4])) << 4) | (((int32_t) (temp[5])) >> 4));

        *temperature = this->_get_compensated_temperature(uncomp_temperature);
        *pressure = this->_get_compensated_pressure(uncomp_pressure) / 100.0; // Convert Pa to mbar (hPa)

        Task::delay_ms(this->get_measure_time());
    }

    return ret;
}
