#include <utils/attitude.h>

#define I2C_MASTER_NUM      I2C_NUM_1         /*!< I2C port number for master dev */

Attitude::Attitude(float period)
{
    _ahrs   = new MadgwickAhrs(period);
    _height = new HeightObserver(period);

    _i2c    = new I2cMaster(I2C_MASTER_NUM);
    _marg   = new Marg(_i2c);

    _period = period;

    _roll        = 0.0f;
    _roll_speed  = 0.0f;
    _pitch       = 0.0f;
    _pitch_speed = 0.0f;
    _yaw         = 0.0f;
    _yaw_speed   = 0.0f;

    _i2c->init();
    _marg->init();
}

void Attitude::update(void)
{
    float gx, gy, gz;           /* gyro in drone frame (sensor data) */
    float ax, ay, az;           /* accelero in drone frame (sensor data) */
    float ax_r, ay_r, az_r;     /* accelero in earth frame */
    float mx, my, mz;           /* magneto in drone frame (sensor data) */
    float baro;                 /* barometer sensor data */

    _marg->read_acc(&ax, &ay, &az);
    _marg->read_gyro(&gx, &gy, &gz);
    _marg->read_mag(&mx, &my, &mz);
    _baro->read(&baro);

    _ahrs->update(gx, gy, gz, ax, ay, az, mx, my, mz);
    _ahrs->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);
    _height->update(az_r, baro, 0.0);

    _roll_speed  = (_ahrs->roll()  - _roll)  / _period;
    _pitch_speed = (_ahrs->pitch() - _pitch) / _period;
    _yaw_speed   = (_ahrs->yaw()   - _yaw)   / _period;

    _roll  = _ahrs->roll();
    _pitch = _ahrs->pitch();
    _yaw   = _ahrs->yaw();
}

float Attitude::roll(void)
{
    return _roll;
}

float Attitude::roll_speed(void)
{
    return _roll_speed;
}

float Attitude::pitch(void)
{
    return _pitch;
}

float Attitude::pitch_speed(void)
{
    return _pitch_speed;
}

float Attitude::yaw(void)
{
    return _yaw;
}

float Attitude::yaw_speed(void)
{
    return _yaw_speed;
}

float Attitude::height(void)
{
    return _height->height();
}

float Attitude::vertical_speed(void)
{
    return _height->vertical_speed();
}
