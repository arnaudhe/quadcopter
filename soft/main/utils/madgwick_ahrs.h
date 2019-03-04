#pragma once

#include <utils/ahrs.h>
#include <utils/matrix.h>
#include <utils/quaternion.h>

using namespace std;

class MadgwickAhrs : public Ahrs
{
  
  private: 

    /* Attributes */

    float _period;
    float _beta;               ///< 2 * proportional gain (Kp)
    Quaternion _q;

    /* Methods */

    Vect compute_F(Quaternion acc, Quaternion mag, Quaternion B);
    Matrix compute_J(Quaternion B);

  public:

    /* Constructors */

    MadgwickAhrs(float period);
    ~MadgwickAhrs();

    /* Accessors*/

    /* Other methods */

    void update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

    float roll(void);
    float pitch(void);
    float yaw(void);

    void rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r);

};
