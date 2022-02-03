#pragma once

#include <periph/i2c_master.h>
#include <drv/BMP180.h>
#include <drv/BMP280.h>
#include <os/task.h>
#include <os/mutex.h>
#include <platform.h>

class Barometer : public Task
{
  private:

    I2cMaster                * _i2c;
    double                     _temperature;
    double                     _initial_pressure;
    double                     _height;
    PLATFORM_BAROMETER_MODEL * _bmp;
    Mutex                      _mutex;

    void run();

    // Given a pressure P (mb) taken at a specific altitude (meters),
    // return the equivalent pressure (mb) at sea level.
    // This produces pressure readings that can be used for weather measurements.
    double sea_level(double P, double A);

    // Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
    // return altitude (meters) above baseline.
    double altitude(double P, double P0);

  public:

    Barometer(I2cMaster * i2c);

    void init(void);

    float height();

};