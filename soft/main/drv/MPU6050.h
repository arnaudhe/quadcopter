#pragma once

#include <periph/i2c_master.h>

using namespace std;

class MPU6050
{
  
  private: 

    /* Attributes */

    I2cMaster  * _i2c;      ///< i2c device
    uint8_t      _address;  ///< i2c address

    esp_err_t _read_sensor(uint8_t reg, float *x, float *y, float *z, float scale);

  public:

    /* Constructors */

    MPU6050(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_acc(float * acc_x, float * acc_y, float * acc_z);

    esp_err_t read_gyro(float * gyro_x, float * gyro_y, float * gyro_z);

    esp_err_t read_acc_gyro(float *acc_x, float *acc_y, float *acc_z,
                           float *gyro_x, float *gyro_y, float *gyro_z);

    esp_err_t set_i2c_master_mode_enabled(bool enabled);

    esp_err_t set_i2c_bypass_enabled(bool enabled);

};