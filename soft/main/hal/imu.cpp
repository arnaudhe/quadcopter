#include "imu.h"

void marg::marg(i2c_master * i2c)
{
    _i2c = i2c;
    _mpu = new MPU6050(i2c);
}

esp_err_t init(void)
{
    return mpu->init();
}

esp_err_t read_acc(float * acc_x, float * acc_y, float * acc_z)
{
    return mpu->read_acc(acc_x, acc_y, acc_z);
}

esp_err_t read_gyro(float * gyro_x, float * gyro_y, float * gyro_z)
{
    return mpu->read_gyro(gyro_x, gyro_y, gyro_z);
}