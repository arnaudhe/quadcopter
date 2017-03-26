#pragma once

#include "periph/i2c_master.h"

using namespace std;

class HMC5883L
{
  
  private: 

    /* Attributes */

    i2c_master * _i2c;      ///< i2c device

  public:

    /* Constructors */

    HMC5883L(i2c_master * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read_mag(float * mag_x, float * mag_y, float * mag_z);

};