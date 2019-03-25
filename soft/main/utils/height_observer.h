#pragma once

#include <utils/matrix.h>
#include <utils/kalman.h>

using namespace std;

class HeightObserver : public Kalman
{

  private: 

    /* Attributes */

    /* Methods */

  public:

    /* Constructors */

    HeightObserver(float period);

    /* Accessors */

    float height(void);
    float vertical_speed(void);

    /* Other methods */

    void update(float acc_z, float baro, float ultrasound);

};
