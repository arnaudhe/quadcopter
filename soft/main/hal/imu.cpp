#include <hal/imu.h>

imu::imu(i2c_master * i2c)
{
    _i2c = i2c;
    _mpu = new MPU6050(_i2c);
}

esp_err_t imu::init(void)
{
    return _mpu->init();
}

esp_err_t imu::read_acc(float * acc_x, float * acc_y, float * acc_z)
{
    return _mpu->read_acc(acc_x, acc_y, acc_z);
}

esp_err_t imu::read_gyro(float * gyro_x, float * gyro_y, float * gyro_z)
{
    return _mpu->read_gyro(gyro_x, gyro_y, gyro_z);
}