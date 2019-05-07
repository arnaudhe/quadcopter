#include <hal/imu.h>

imu::imu(I2cMaster * i2c)
{
    _i2c = i2c;
    _mpu = new MPU6050(_i2c);
}

esp_err_t imu::init(void)
{
    return _mpu->init();
}

esp_err_t imu::read_acc(double * acc_x, double * acc_y, double * acc_z)
{
    return _mpu->read_acc(acc_x, acc_y, acc_z);
}

esp_err_t imu::read_gyro(double * gyro_x, double * gyro_y, double * gyro_z)
{
    return _mpu->read_gyro(gyro_x, gyro_y, gyro_z);
}