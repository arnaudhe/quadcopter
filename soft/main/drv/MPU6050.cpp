#include <drv/MPU6050.h>
#include <drv/MPU6050_defs.h>
#include <drv/MPU6050_conf.h>
#include <hal/log.h>
#include <esp_attr.h>

#include <os/task.h>

#define DEG_TO_RAD(a) ((a * 3.14159265359) / (180.0))
#define RAD_TO_DEG    (180.0 / 3.14159265359)

MPU6050::MPU6050(I2cMaster * i2c)
{
    _i2c     = i2c;
    _address = MPU6050_ADDRESS;
}

esp_err_t MPU6050::init(void)
{
    uint8_t   current;
    uint8_t   count = 0;
    esp_err_t ret;
    
    do
    {
        Task::delay_ms(MPU6050_WAKEUP_DELAY_ms);

        ret = _i2c->write_bit(_address, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, false);

        if (ret == ESP_OK)
        {
            _i2c->write_register(_address, MPU6050_RA_CONFIG, (MPU6050_LOW_PASS_FILTER_SEL << MPU6050_CFG_DLPF_CFG_BIT) | (MPU6050_EXT_SYNC_DISABLED << MPU6050_CFG_EXT_SYNC_SET_BIT)) ;
            _i2c->read_register(_address, MPU6050_RA_CONFIG, &current );

            _i2c->write_register(_address, MPU6050_RA_GYRO_CONFIG, (MPU6050_GYRO_SENSIVITY_SEL << MPU6050_GCONFIG_FS_SEL_BIT));
            _i2c->read_register(_address, MPU6050_RA_GYRO_CONFIG, &current );

            _i2c->write_register(_address, MPU6050_RA_ACCEL_CONFIG, (MPU6050_ACC_SENSIVITY_SEL << MPU6050_ACONFIG_AFS_SEL_BIT));
            _i2c->read_register(_address, MPU6050_RA_ACCEL_CONFIG, &current );

            LOG_INFO("Success");

            return ESP_OK;
        }
    }while (count++ < 3);

    return ret;
}

esp_err_t IRAM_ATTR MPU6050::_read_sensor(uint8_t reg, float * x, float * y, float * z, float scale)
{
    uint8_t buffer[6];
    int16_t temp;

    if (ESP_OK == _i2c->read_registers(_address, reg, buffer, 6))
    {
        temp = (buffer[0] << 8) + buffer[1];
        *x = (float)(temp) / scale;

        temp = (buffer[2] << 8) + buffer[3];
        *y = (float)(temp) / scale;

        temp = (buffer[4] << 8) + buffer[5];
        *z = (float)(temp) / scale;

        return ESP_OK;
    }
    else
    {
        LOG_ERROR("Failed to read values at 0x%02x", reg);
        return ESP_FAIL;
    }
}

esp_err_t IRAM_ATTR MPU6050::read_acc(float * acc_x, float * acc_y, float * acc_z)
{
    return this->_read_sensor(MPU6050_RA_ACCEL_XOUT_H, acc_x, acc_y, acc_z, MPU6050_ACC_SENSIVITY);
}

esp_err_t IRAM_ATTR MPU6050::read_gyro(float * gyro_x, float * gyro_y, float * gyro_z)
{
    return this->_read_sensor(MPU6050_RA_GYRO_XOUT_H, gyro_x, gyro_y, gyro_z, RAD_TO_DEG * MPU6050_GYRO_SENSIVITY);
}

esp_err_t IRAM_ATTR MPU6050::read_acc_gyro(float *acc_x, float *acc_y, float *acc_z,
                                 float *gyro_x, float *gyro_y, float *gyro_z)
{
    uint8_t buffer[14];
    int16_t temp;

    if (ESP_OK == _i2c->read_registers(_address, MPU6050_RA_ACCEL_XOUT_H, buffer, 14))
    {
        temp = (buffer[0] << 8) + buffer[1];
        *acc_x = (float)(temp) / MPU6050_ACC_SENSIVITY;

        temp = (buffer[2] << 8) + buffer[3];
        *acc_y = (float)(temp) / MPU6050_ACC_SENSIVITY;

        temp = (buffer[4] << 8) + buffer[5];
        *acc_z = (float)(temp) / MPU6050_ACC_SENSIVITY;

        temp = (buffer[8] << 8) + buffer[9];
        *gyro_x = (float)(temp) / (RAD_TO_DEG * MPU6050_GYRO_SENSIVITY);

        temp = (buffer[10] << 8) + buffer[11];
        *gyro_y = (float)(temp) / (RAD_TO_DEG * MPU6050_GYRO_SENSIVITY);

        temp = (buffer[12] << 8) + buffer[13];
        *gyro_z = (float)(temp) / (RAD_TO_DEG * MPU6050_GYRO_SENSIVITY);

        return ESP_OK;
    }
    else
    {
        LOG_ERROR("Failed to read values of both sensors");
        return ESP_FAIL;
    }
}

esp_err_t MPU6050::set_i2c_master_mode_enabled(bool enabled) 
{
    return _i2c->write_bit(_address, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

esp_err_t MPU6050::set_i2c_bypass_enabled(bool enabled) 
{
    return _i2c->write_bit(_address, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}
