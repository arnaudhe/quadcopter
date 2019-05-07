#pragma once

#include <periph/i2c_master.h>
#include <drv/MPU6050.h>
#include <drv/HMC5883L.h>

using namespace std;

class Marg
{

  private: 

    /* Attributes */

    I2cMaster  * _i2c;      ///< i2c device
    MPU6050    * _mpu;
    HMC5883L   * _hmc;

  public:

    /* Constructors */

    Marg(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_acc(double * acc_x, double * acc_y, double * acc_z);

    esp_err_t read_gyro(double * gyro_x, double * gyro_y, double * gyro_z);

    esp_err_t read_mag(double * mag_x, double * mag_y, double * mag_z);

};