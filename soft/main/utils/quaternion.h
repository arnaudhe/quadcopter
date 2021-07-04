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
    Quaternion(float angle);
    Quaternion(Matrix mat);
    Quaternion(float q0, float q1, float q2, float q3);

    void set(float q0, float q1, float q2, float q3);

    Quaternion operator+(Quaternion);
    Quaternion operator-(Quaternion);
    Quaternion operator*(Quaternion);
    Quaternion operator*(float);

    Quaternion conjugate(void);

    float roll(void);
    float pitch(void);
    float yaw(void);

    Matrix rotation_matrix(void);
};