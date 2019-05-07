#include <hal/marg.h>
#include <hal/log.h>

Marg::Marg(I2cMaster * i2c)
{
    _i2c = i2c;
    _mpu = new MPU6050(_i2c);
    _hmc = new HMC5883L(_i2c);
}

esp_err_t Marg::init(void)  
{
    if (_mpu->init() == ESP_OK)
    {
        _mpu->set_i2c_master_mode_enabled(false);
        _mpu->set_i2c_bypass_enabled(true);
        
        return _hmc->init();
    }

    LOG_ERROR("init failed");

    return ESP_FAIL;
}

esp_err_t Marg::read_acc(float * acc_x, float * acc_y, float * acc_z)
{
    return _mpu->read_acc(acc_x, acc_y, acc_z);
}

esp_err_t Marg::read_gyro(float * gyro_x, float * gyro_y, float * gyro_z)
{
    return _mpu->read_gyro(gyro_x, gyro_y, gyro_z);
}

esp_err_t Marg::read_mag(float * mag_x, float * mag_y, float * mag_z)
{
    return _hmc->read_mag(mag_x, mag_y, mag_z);
}