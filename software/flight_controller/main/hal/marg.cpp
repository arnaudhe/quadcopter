#include <hal/marg.h>
#include <hal/log.h>
#include <esp_attr.h>

Marg::Marg(I2cMaster * i2c)
{
    _i2c = i2c;
    _mpu = new MPU6050(_i2c);
    _hmc = new HMC5883L(_i2c);
}

esp_err_t Marg::init(void)
{
    _mutex = new Mutex();

    if (_mpu->init() == ESP_OK)
    {
        _mpu->set_i2c_master_mode_enabled(false);
        _mpu->set_i2c_bypass_enabled(true);
        
        return _hmc->init();
    }

    LOG_ERROR("init failed");

    return ESP_FAIL;
}

esp_err_t IRAM_ATTR Marg::read_acc(float * acc_x, float * acc_y, float * acc_z)
{
    esp_err_t ret;

    _mutex->lock();
    ret = _mpu->read_acc(acc_x, acc_y, acc_z);
    _mutex->unlock();

    return ret;
}

esp_err_t IRAM_ATTR Marg::read_gyro(float * gyro_x, float * gyro_y, float * gyro_z)
{
    esp_err_t ret;

    _mutex->lock();
    ret = _mpu->read_gyro(gyro_x, gyro_y, gyro_z);
    _mutex->unlock();

    return ret;
}

esp_err_t IRAM_ATTR Marg::read_acc_gyro(float *acc_x,  float *acc_y,  float *acc_z,
                                        float *gyro_x, float *gyro_y, float *gyro_z)
{
    esp_err_t ret;

    _mutex->lock();
    ret = _mpu->read_acc_gyro(acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z);
    _mutex->unlock();

    return ret;
}

esp_err_t IRAM_ATTR Marg::read_mag(float * mag_x, float * mag_y, float * mag_z)
{
    esp_err_t ret;

    _mutex->lock();
    ret = _hmc->read_mag(mag_x, mag_y, mag_z);
    _mutex->unlock();

    return ret;
}