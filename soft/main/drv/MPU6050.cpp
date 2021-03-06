#include <drv/MPU6050.h>
#include <drv/MPU6050_defs.h>
#include <drv/MPU6050_conf.h>

#include <os/task.h>

#define DEG_TO_RAD(a) ((a * 3.1416) / (180.0))

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

            return ESP_OK;
        }
        
    }while (count++ < 3);

    return ret;
}

esp_err_t MPU6050::read_acc(double * acc_x, double * acc_y, double * acc_z)
{
    uint8_t high, low;
    int16_t temp;

    _i2c->read_register( _address, MPU6050_RA_ACCEL_XOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_ACCEL_XOUT_L, &low);
    temp = (high << 8) + low;
    *acc_x = (double)((int)temp) / MPU6050_ACC_SENSIVITY;

    _i2c->read_register( _address, MPU6050_RA_ACCEL_YOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_ACCEL_YOUT_L, &low);
    temp = (high << 8) + low;
    *acc_y = (double)((int)temp) / MPU6050_ACC_SENSIVITY;

    _i2c->read_register( _address, MPU6050_RA_ACCEL_ZOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_ACCEL_ZOUT_L, &low);
    temp = (high << 8) + low;
    *acc_z = (double)((int)temp) / MPU6050_ACC_SENSIVITY;

    return ESP_OK;
}

esp_err_t MPU6050::read_gyro(double * gyro_x, double * gyro_y, double * gyro_z)
{
    uint8_t high, low;
    int16_t temp;

    _i2c->read_register( _address, MPU6050_RA_GYRO_XOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_GYRO_XOUT_H, &low);
    temp = (high << 8) + low;
    *gyro_x = DEG_TO_RAD((double)((int)temp) / MPU6050_GYRO_SENSIVITY);

    _i2c->read_register( _address, MPU6050_RA_GYRO_YOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_GYRO_YOUT_L, &low);
    temp = (high << 8) + low;
    *gyro_y = DEG_TO_RAD((double)((int)temp) / MPU6050_GYRO_SENSIVITY);

    _i2c->read_register( _address, MPU6050_RA_GYRO_ZOUT_H, &high);
    _i2c->read_register( _address, MPU6050_RA_GYRO_ZOUT_L, &low);
    temp = (high << 8) + low;
    *gyro_z = DEG_TO_RAD((double)((int)temp) / MPU6050_GYRO_SENSIVITY);

    return ESP_OK;
}

esp_err_t MPU6050::set_i2c_master_mode_enabled(bool enabled) 
{
    return _i2c->write_bit(_address, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

esp_err_t MPU6050::set_i2c_bypass_enabled(bool enabled) 
{
    return _i2c->write_bit(_address, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}
