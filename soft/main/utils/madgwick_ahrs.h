#pragma once

using namespace std;

class marg
{
  
  private: 

    /* Attributes */

    float _sampling_frequency;
    float _beta;               ///< 2 * proportional gain (Kp)
    float _q0, _q1, _q2, _q3;  ///< quaternion of sensor frame relative to auxiliary frame

    /* Methods */

    float inv_sqrt(float x);

  public:

    /* Constructors */

    madgwick_ahrs(float sampling_frequency);

    /* Accessors*/

    /* Other methods */

    void update_marg(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

    void update_imu(float gx, float gy, float gz, float ax, float ay, float az);

    void get_euler(float * phi, float * theta, float * psi);

};
