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

esp_err_t Marg::read_acc(double * acc_x, double * acc_y, double * acc_z)
{
    return _mpu->read_acc(acc_x, acc_y, acc_z);
}

esp_err_t Marg::read_gyro(double * gyro_x, double * gyro_y, double * gyro_z)
{
    return _mpu->read_gyro(gyro_x, gyro_y, gyro_z);
}

esp_err_t Marg::read_mag(double * mag_x, double * mag_y, double * mag_z)
{
    return _hmc->read_mag(mag_x, mag_y, mag_z);
}