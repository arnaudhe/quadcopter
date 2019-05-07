#pragma once

#include <utils/matrix.h>
#include <utils/quaternion.h>

using namespace std;

class Madgwick
{
  
  private: 

    /* Attributes */

    double _period;
    double _beta;               ///< 2 * proportional gain (Kp)
    Quaternion _q;

    /* Methods */

    Vect compute_F(Quaternion acc, Quaternion mag, Quaternion B);
    Matrix compute_J(Quaternion B);

  public:

    /* Constructors */

    Madgwick(double period, double gain);
    ~Madgwick();

    /* Accessors*/

    /* Other methods */

    void update(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz);

    double roll(void);
    double pitch(void);
    double yaw(void);

    void rotate(double x, double y, double z, double * x_r, double * y_r, double * z_r);

};
