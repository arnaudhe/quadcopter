#include <math.h>
#include <iostream>
#include <utils/quaternion.h>

Quaternion::Quaternion():
    Vect(4, 0.0)
{
    set(1.0f, 0.0f, 0.0f, 0.0f);
}

Quaternion::Quaternion(Matrix mat):
    Vect(mat)
{
}

Quaternion::Quaternion(float angle):
    Vect(4, 0.0)
{
    set(cos(angle/2.0), -sin(angle/2.0), -sin(angle/2.0), -sin(angle/2.0));
}

Quaternion::Quaternion(float q0, float q1, float q2, float q3):
    Vect(4, 0.0)
{
    set(q0, q1, q2, q3);
}

void Quaternion::set(float q0, float q1, float q2, float q3)
{
    Vect::set(0, q0);
    Vect::set(1, q1);
    Vect::set(2, q2);
    Vect::set(3, q3);
}

float Quaternion::roll(void)
{
    return atan2f(2.0f * (_data[0][0] * _data[1][0] + _data[2][0] * _data[3][0]), 1.0f - 2.0f * (_data[1][0] * _data[1][0] + _data[2][0] * _data[2][0]));   
}

float Quaternion::pitch(void)
{
    return asinf(2.0f * (_data[0][0] * _data[2][0] - _data[1][0] * _data[3][0]));
}

float Quaternion::yaw(void)
{
    return atan2f(2.0f * (_data[0][0] * _data[3][0] + _data[1][0] * _data[2][0]), 1.0f - 2.0f * (_data[2][0] * _data[2][0] + _data[3][0] * _data[3][0]));
}

Quaternion Quaternion::conjugate(void)
{
    return Quaternion(_data[0][0], -_data[1][0], -_data[2][0], -_data[3][0]);
}

Quaternion Quaternion::operator+(Quaternion B)
{
    return Matrix::operator+(B);
}

Quaternion Quaternion::operator-(Quaternion B)
{
    return Matrix::operator-(B);
}

Quaternion Quaternion::operator*(Quaternion B)
{
    return Quaternion(_data[0][0] * B(0) - _data[1][0] * B(1) - _data[2][0] * B(2) - _data[3][0] * B(3),
                      _data[0][0] * B(1) + _data[1][0] * B(0) + _data[2][0] * B(3) - _data[3][0] * B(2),
                      _data[0][0] * B(2) - _data[1][0] * B(3) + _data[2][0] * B(0) + _data[3][0] * B(1),
                      _data[0][0] * B(3) + _data[1][0] * B(2) - _data[2][0] * B(1) + _data[3][0] * B(0));
}

Quaternion Quaternion::operator*(float k)
{
    return Matrix::operator*(k);
}

Matrix Quaternion::rotation_matrix(void)
{
    Matrix R = Matrix(3, 3);

    R.set(0, 0, 2.0f * _data[0][0] * _data[0][0] - 1.0f + 2.0f * _data[1][0] * _data[1][0]);
    R.set(0, 1, 2.0f * (_data[1][0] * _data[2][0] + _data[0][0] * _data[3][0]));
    R.set(0, 2, 2.0f * (_data[1][0] * _data[3][0] - _data[0][0] * _data[2][0]));
    R.set(1, 0, 2.0f * (_data[1][0] * _data[2][0] - _data[0][0] * _data[3][0]));
    R.set(1, 1, 2.0f * _data[0][0] * _data[0][0] - 1.0f + 2.0f * _data[2][0] * _data[2][0]);
    R.set(1, 2, 2.0f * (_data[2][0] * _data[3][0] + _data[0][0] * _data[1][0]));
    R.set(2, 0, 2.0f * (_data[1][0] * _data[3][0] + _data[0][0] * _data[2][0]));
    R.set(2, 1, 2.0f * (_data[2][0] * _data[3][0] - _data[0][0] * _data[1][0]));
    R.set(2, 2, 2.0f * _data[0][0] * _data[0][0] - 1.0f + 2.0f * _data[3][0] * _data[3][0]);

    return R;
}