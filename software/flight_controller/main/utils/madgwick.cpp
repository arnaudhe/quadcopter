#include <utils/madgwick.h>
#include <math.h>
#include <iostream>
#include <esp_attr.h>

Madgwick::Madgwick(float period, float gain):
    _q()
{
    _period = period;
    _beta   = gain;
}

Vect IRAM_ATTR Madgwick::compute_F(Quaternion acc, Quaternion mag, Quaternion B)
{
    Vect F = Vect(6);

    F.set(0, 2.0f * (       _q(1)*_q(3) - _q(0)*_q(2)) - acc(1));
    F.set(1, 2.0f * (       _q(0)*_q(1) + _q(2)*_q(3)) - acc(2));
    F.set(2, 2.0f * (0.5f - _q(1)*_q(1) - _q(2)*_q(2)) - acc(3));
    F.set(3, 2.0f * B(1) *  (0.5f - _q(2)*_q(2) - _q(3)*_q(3)) + 2.0f * B(3) * (       _q(1)*_q(3) - _q(0)*_q(2)) - mag(1));
    F.set(4, 2.0f * B(1) *  (       _q(1)*_q(2) - _q(0)*_q(3)) + 2.0f * B(3) * (       _q(0)*_q(1) + _q(2)*_q(3)) - mag(2));
    F.set(5, 2.0f * B(1) *  (       _q(0)*_q(2) + _q(1)*_q(3)) + 2.0f * B(3) * (0.5f - _q(1)*_q(1) - _q(2)*_q(2)) - mag(3));

    return F;
}

Matrix IRAM_ATTR Madgwick::compute_J(Quaternion B)
{
    Matrix J = Matrix(6, 4);

    J.set(0, 0, -2.0f * _q(2));
    J.set(0, 1,  2.0f * _q(3));
    J.set(0, 2, -2.0f * _q(0));
    J.set(0, 3,  2.0f * _q(1));

    J.set(1, 0,  2.0f * _q(1));
    J.set(1, 1,  2.0f * _q(0));
    J.set(1, 2,  2.0f * _q(3));
    J.set(1, 3,  2.0f * _q(2));

    J.set(2, 0,  0.0f) ;
    J.set(2, 1, -4.0f * _q(1));
    J.set(2, 2,  2.0f * _q(3));
    J.set(2, 3,  2.0f * _q(2));

    J.set(3, 0, -2.0f * B(3) * _q(2));
    J.set(3, 1,  2.0f * B(3) * _q(3));
    J.set(3, 2, -4.0f * B(1) * _q(2) - 2.0f * B(3) * _q(0));
    J.set(3, 3, -4.0f * B(1) * _q(3) + 2.0f * B(3) * _q(1));

    J.set(4, 0, -2.0f * B(1) * _q(3) + 2.0f * B(3) * _q(1));
    J.set(4, 1,  2.0f * B(1) * _q(2) + 2.0f * B(3) * _q(0));
    J.set(4, 2,  2.0f * B(1) * _q(1) + 2.0f * B(3) * _q(3));
    J.set(4, 3, -2.0f * B(1) * _q(0) + 2.0f * B(3) * _q(2));

    J.set(5, 0,  2.0f * B(1) * _q(2));
    J.set(5, 1,  2.0f * B(1) * _q(3) - 4.0f * B(3) * _q(1));
    J.set(5, 2,  2.0f * B(1) * _q(0) - 4.0f * B(3) * _q(2));
    J.set(5, 3,  2.0f * B(1) * _q(1));

    return J;
}

void IRAM_ATTR Madgwick::update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) 
{
    Quaternion gyr = Quaternion(0.0f, gx, gy, gz);
    Quaternion acc = Quaternion(0.0f, ax, ay, az);
    Quaternion mag = Quaternion(0.0f, mx, my, mz);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(acc.norm() != 0.0 && mag.norm() != 0.0f)
    {
        acc.normalize();
        mag.normalize();

        /* Reference direction of Earth's magnetic field */
        Quaternion H    = _q * (mag * _q.conjugate());
        Quaternion B    = Quaternion(0.0f, sqrtf(H(1) * H(1) + H(2) * H(2)), 0.0f, H(3));

        /* Gradient decent algorithm corrective step */
        Vect F          = compute_F(acc, mag, B);
        Matrix J        = compute_J(B);
        Quaternion step = Quaternion(J.transpose() * F);
        step.normalize();

        /* AHRS Quaternion iteration (integration + correction) */
        _q = _q + (((_q * gyr) * 0.5f) - (step * _beta)) * _period;
        _q.normalize();
    }
}

float IRAM_ATTR Madgwick::roll(void)
{
    return _q.roll();
}

float IRAM_ATTR Madgwick::pitch(void)
{
    return _q.pitch();
}

float Madgwick::yaw(void)
{
    return _q.yaw();
}

void IRAM_ATTR Madgwick::rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r)
{
    Quaternion v = Quaternion(4);

    v.set(0.0, x, y, z);

    Quaternion v_r = _q * v * _q.conjugate();

    *x_r = v_r(1);
    *y_r = v_r(2);
    *z_r = v_r(3);
}

Madgwick::~Madgwick()
{
    return;
}
