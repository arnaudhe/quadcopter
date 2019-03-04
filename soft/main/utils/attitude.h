#pragma once

#include <utils/ahrs.h>
#include <utils/madgwick_ahrs.h>
#include <utils/height_observer.h>
#include <hal/marg.h>
#include <hal/barometer.h>
#include <periph/i2c_master.h>

using namespace std;

class Attitude
{

  private:

    /* Attributes */

    I2cMaster      * _i2c;
    Ahrs           * _ahrs;
    HeightObserver * _height;
    Marg           * _marg;
    Barometer      * _baro;

    float _period;

    float _roll;
    float _roll_speed;
    float _pitch;
    float _pitch_speed;
    float _yaw;
    float _yaw_speed;

  public:

    /* Constructors */

    Attitude(float period);

    /* Accessors */

    float roll(void);
    float roll_speed(void);

    float pitch(void);
    float pitch_speed(void);

    float yaw(void);
    float yaw_speed(void);

    float height(void);
    float vertical_speed(void);

    /* Other methods */

    void update(void);

};
