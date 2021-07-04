#pragma once

#include <periph/i2c_master.h>

using namespace std;

class HMC5883L
{
  
  private: 

    /* Attributes */

    I2cMaster  * _i2c;      ///< i2c device
    uint8_t      _address;  ///< i2c address

  public:

    /* Constructors */

    HMC5883L(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_mag(float * mag_x, float * mag_y, float * mag_z);

};