#pragma once

#include "periph/i2c_master.h"
#include "drv/MPU6050.h"
#include "drv/HMC5883L.h"

using namespace std;

class marg
{
  
  private: 

    /* Attributes */

    i2c_master * _i2c;      ///< i2c device
    MPU6050    * _mpu;
    HMC5883L   * _hmc;

  public:

    /* Constructors */

    marg(i2c_master * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_acc(float * acc_x, float * acc_y, float * acc_z);

    esp_err_t read_gyro(float * gyro_x, float * gyro_y, float * gyro_z);

    esp_err_t read_mag(float * mag_x, float * mag_y, float * mag_z);

};