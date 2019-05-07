#pragma once

#include <vector>
#include <string>

#include <utils/matrix.h>
#include <utils/vector.h>

using namespace std;

class Quaternion : public Vect
{
  public:

    Quaternion();
    Quaternion(double angle);
    Quaternion(Matrix mat);
    Quaternion(double q0, double q1, double q2, double q3);

    void set(double q0, double q1, double q2, double q3);

    Quaternion operator+(Quaternion);
    Quaternion operator-(Quaternion);
    Quaternion operator*(Quaternion);
    Quaternion operator*(double);

    Quaternion conjugate(void);

    double roll(void);
    double pitch(void);
    double yaw(void);

    Matrix rotation_matrix(void);
};