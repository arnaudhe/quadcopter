#pragma once

#include <periph/i2c_master.h>

using namespace std;

class MPU6050
{
  
  private: 

    /* Attributes */

    I2cMaster  * _i2c;      ///< i2c device
    uint8_t      _address;  ///< i2c address

  public:

    /* Constructors */

    MPU6050(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_acc(double * acc_x, double * acc_y, double * acc_z);

    esp_err_t read_gyro(double * gyro_x, double * gyro_y, double * gyro_z);

    esp_err_t set_i2c_master_mode_enabled(bool enabled);

    esp_err_t set_i2c_bypass_enabled(bool enabled);

};