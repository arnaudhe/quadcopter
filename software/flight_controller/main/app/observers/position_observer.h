#pragma once

#include <utils/matrix.h>
#include <utils/kalman.h>

using namespace std;

class PositionObserver : public Kalman
{

  private: 

    /* Attributes */

    /* Methods */

  public:

    /* Constructors */

    PositionObserver(float period);

    /* Accessors */

    float x(void);
    float x_speed(void);
    float y(void);
    float y_speed(void);


    /* Other methods */

    void update(float acc_x, float acc_y, float gps_x, float gps_y);

};
