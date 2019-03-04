#pragma once

#include <periph/i2c_master.h>

using namespace std;

class BMP180
{
  
  private: 

    /* Attributes */

    I2cMaster  * _i2c;      ///< i2c device
    uint8_t      _address;  ///< i2c address

  public:

    /* Constructors */

    BMP180(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);

    esp_err_t read(float * baro);

};