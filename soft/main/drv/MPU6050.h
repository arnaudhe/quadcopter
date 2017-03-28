#pragma once

#include <periph/i2c_master.h>

using namespace std;

class MPU6050
{
  
  private: 

    /* Attributes */

    i2c_master * _i2c;      ///< i2c device
    uint8_t      _address;  ///< i2c address

  public:

    /* Constructors */

    MPU6050(i2c_master * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_acc(float * acc_x, float * acc_y, float * acc_z);

    esp_err_t read_gyro(float * gyro_x, float * gyro_y, float * gyro_z);

    esp_err_t set_i2c_master_mode_enabled(bool enabled);

    esp_err_t set_i2c_bypass_enabled(bool enabled);

};