#include <math.h>
#include <iostream>
#include <utils/quaternion.h>
#include <esp_attr.h>

IRAM_ATTR Quaternion::Quaternion():
    Vect(4, 0.0f)
{
    set(1.0f, 0.0f, 0.0f, 0.0f);
}

Quaternion::Quaternion(Matrix mat):
    Vect(mat)
{
}

IRAM_ATTR Quaternion::Quaternion(float angle):
    Vect(4, 0.0f)
{
    set(cos(angle/2.0f), -sin(angle/2.0f), -sin(angle/2.0f), -sin(angle/2.0f));
}

IRAM_ATTR Quaternion::Quaternion(float q0, float q1, float q2, float q3):
    Vect(4, 0.0f)
{
    set(q0, q1, q2, q3);
}

void IRAM_ATTR Quaternion::set(float q0, float q1, float q2, float q3)
{
    Vect::set(0, q0);
    Vect::set(1, q1);
    Vect::set(2, q2);
    Vect::set(3, q3);
}

float IRAM_ATTR Quaternion::roll(void)
{
    return atan2(2.0f * (_data[0][0] * _data[1][0] + _data[2][0] * _data[3][0]), 1.0f - 2.0f * (_data[1][0] * _data[1][0] + _data[2][0] * _data[2][0]));   
}

float IRAM_ATTR Quaternion::pitch(void)
{
    return asin(2.0f * (_data[0][0] * _data[2][0] - _data[1][0] * _data[3][0]));
}

float IRAM_ATTR Quaternion::yaw(void)
{
    return atan2f(2.0f * (_data[0][0] * _data[3][0] + _data[1][0] * _data[2][0]), 1.0f - 2.0f * (_data[2][0] * _data[2][0] + _data[3][0] * _data[3][0]));
}

Quaternion IRAM_ATTR Quaternion::conjugate(void)
{
    return Quaternion(_data[0][0], -_data[1][0], -_data[2][0], -_data[3][0]);
}

Quaternion IRAM_ATTR Quaternion::operator+(Quaternion B)
{
    return Matrix::operator+(B);
}

Quaternion IRAM_ATTR Quaternion::operator-(Quaternion B)
{
    return Matrix::operator-(B);
}

Quaternion IRAM_ATTR Quaternion::operator*(Quaternion B)
{
    return Quaternion(_data[0][0] * B(0) - _data[1][0] * B(1) - _data[2][0] * B(2) - _data[3][0] * B(3),
                      _data[0][0] * B(1) + _data[1][0] * B(0) + _data[2][0] * B(3) - _data[3][0] * B(2),
                      _data[0][0] * B(2) - _data[1][0] * B(3) + _data[2][0] * B(0) + _data[3][0] * B(1),
                      _data[0][0] * B(3) + _data[1][0] * B(2) - _data[2][0] * B(1) + _data[3][0] * B(0));
}

Quaternion IRAM_ATTR Quaternion::operator*(float k)
{
    return Matrix::operator*(k);
}

Matrix IRAM_ATTR Quaternion::rotation_matrix(void)
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