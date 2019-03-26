#include <app/controllers/attitude_controller.h>
#include <app/controllers/attitude_controller_conf.h>

AttitudeController::AttitudeController(float period)
{
    _period = period;

    _euler_observer = new EulerObserver(period);
    _height_observer = new HeightObserver(period);

    _i2c    = new I2cMaster(I2C_MASTER_NUM);
    _marg   = new Marg(_i2c);

    _i2c->init();
    _marg->init();

    _height_controller = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP),
                                        new Pid(period, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP));

    _roll_controller   = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP),
                                        new Pid(period, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP));

    _pitch_controller  = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP),
                                        new Pid(period, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP));

    _yaw_controller    = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP),
                                        new Pid(period, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP));

    _mixer = new Mixer(new Motor(ATTITUDE_MOTOR_FRONT_LEFT_PULSE_CHANNEL,  ATTITUDE_MOTOR_FRONT_LEFT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_CHANNEL, ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_REAR_LEFT_PULSE_CHANNEL,   ATTITUDE_MOTOR_REAR_LEFT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_REAR_RIGHT_PULSE_CHANNEL,  ATTITUDE_MOTOR_REAR_RIGHT_PULSE_PIN));
}

void AttitudeController::update(void)
{
    float  gx, gy, gz;           /* gyro in drone frame (sensor data) */
    float  ax, ay, az;           /* accelero in drone frame (sensor data) */
    float  ax_r, ay_r, az_r;     /* accelero in earth frame */
    float  mx, my, mz;           /* magneto in drone frame (sensor data) */
    double baro;                 /* barometer sensor data */
    float  roll, pitch, yaw;     /* previous values */

    /* Read the sensors */

    _marg->read_acc(&ax, &ay, &az);
    _marg->read_gyro(&gx, &gy, &gz);
    _marg->read_mag(&mx, &my, &mz);
    _baro->read(baro);

    /* Estimate the attitude */

    roll  = _euler_observer->roll();
    pitch = _euler_observer->pitch();
    yaw   = _euler_observer->yaw();

    _euler_observer->update(gx, gy, gz, ax, ay, az, mx, my, mz);
    _euler_observer->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);
    _height_observer->update(az_r, baro, 0.0);

    _roll_speed   = (_euler_observer->roll()  - roll)  / _period;
    _pitch_speed  = (_euler_observer->pitch() - pitch) / _period;
    _yaw_speed    = (_euler_observer->yaw()   - yaw)   / _period;
    _height_speed = _height_observer->vertical_speed();

    /* Compute the controls */

    _height_controller->update (_height_observer->height(), _height_observer->vertical_speed());
    _roll_controller->update   (_euler_observer->roll(),    _roll_speed);
    _pitch_controller->update  (_euler_observer->pitch(),   _pitch_speed);
    _yaw_controller->update    (_euler_observer->yaw(),     _yaw_speed);

    /* Apply the controls to the motors */

    _mixer->update(_height_controller->command(), _roll_controller->command(), _pitch_controller->command(), _yaw_controller->command());
}

void AttitudeController::set_height_target(Controller::Mode mode, float target)
{
    _height_controller->update_target(mode, target);
}

void AttitudeController::set_roll_target(Controller::Mode mode, float target)
{
    _roll_controller->update_target(mode, target);
}

void AttitudeController::set_pitch_target(Controller::Mode mode, float target)
{
    _pitch_controller->update_target(mode, target);
}

void AttitudeController::set_yaw_target(Controller::Mode mode, float target)
{
    _yaw_controller->update_target(mode, target);
}
