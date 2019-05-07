#pragma once

#include <periph/i2c_master.h>
#include <drv/BMP180.h>
#include <os/task.h>
#include <os/mutex.h>

class Barometer : public Task
{
  private:

    I2cMaster * _i2c;
    double      _temperature;
    double      _initial_pressure;
    double      _height;
    BMP180    * _bmp;
    Mutex       _mutex;

    void run();

  public:

    Barometer(I2cMaster * i2c);

    void init(void);

    float height();

};