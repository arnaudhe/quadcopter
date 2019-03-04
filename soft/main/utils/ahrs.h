#pragma once

class Ahrs
{

protected:

    float _period;

public:

    Ahrs(float period) : _period(period) {};
    virtual ~Ahrs() {}

    virtual void update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

    virtual float roll(void);
    virtual float pitch(void);
    virtual float yaw(void);

    virtual void rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r);

};