#pragma once

#include <periph/i2c_master.h>

using namespace std;

class BMP180
{
  
  private: 

    /* Attributes */

    I2cMaster  * _i2c;              ///< i2c device
    uint8_t      _address;          ///< i2c address
    double        _temperature;
    double        _initial_pressure;

		double        c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;

  public:

    /* Constructors */

    BMP180(I2cMaster * i2c);

    /* Accessors*/

    /* Other methods */

    esp_err_t init(void);
  
    // Start a pressure sampling, with the oversampling
    // coefficient BMP180_CONF_OVERSAMPLING. Depending on it, 
    // wait before calling get_pressure to retrieve value 
    esp_err_t start_pressure();

    // Start a temperature sampling. Wait 5 ms before calling
    // get_temperature to retrieve value
    esp_err_t start_temperature();

    // Reads the last sampled 
    esp_err_t get_temperature(double &T);

    // Reads the last sampled and convert it to mbars
    // to get accurate results, temperature must be 
    // aften measured 
    esp_err_t get_pressure(double &P);

    // Given a pressure P (mb) taken at a specific altitude (meters),
    // return the equivalent pressure (mb) at sea level.
    // This produces pressure readings that can be used for weather measurements.
    double sea_level(double P, double A);

    // Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
    // return altitude (meters) above baseline.
    double altitude(double P, double P0);

};