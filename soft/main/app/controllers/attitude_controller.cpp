#include <app/controllers/attitude_controller.h>
#include <app/controllers/attitude_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>

AttitudeController::AttitudeController(float period, DataRessourcesRegistry * registry):
    PeriodicTask("attitude_ctlr", (int)(period * 1000), false)
{
    _period             = period;
    _registry           = registry;

    _euler_observer  = new EulerObserver(period);
    _height_observer = new HeightObserver(period);

    _i2c        = new I2cMaster(I2C_MASTER_NUM);
    _marg       = new Marg(_i2c);
    _baro       = new Barometer(_i2c);
    _ultrasound = new UltrasoundSensor();

    _i2c->init();
    _marg->init();
    _baro->init();

    _registry->internal_set<string>("control.mode", "off");
    _registry->internal_set<float>("control.motors.front_left", 0.0);
    _registry->internal_set<float>("control.motors.front_right", 0.0);
    _registry->internal_set<float>("control.motors.rear_left", 0.0);
    _registry->internal_set<float>("control.motors.rear_right", 0.0);
    _registry->internal_set<float>("control.attitude.roll.position_target", 0.0);
    _registry->internal_set<string>("control.attitude.roll.mode", "off");
    _registry->internal_set<string>("control.attitude.pitch.mode", "off");
    _registry->internal_set<string>("control.attitude.yaw.mode", "off");
    _registry->internal_set<string>("control.attitude.height.mode", "off");
    _registry->internal_set<float>("control.attitude.roll.kp", ATTITUDE_PID_YAW_POSITION_KP);
    _registry->internal_set<float>("control.attitude.roll.ki", ATTITUDE_PID_YAW_POSITION_KI);
    _registry->internal_set<float>("control.attitude.roll.kd", ATTITUDE_PID_YAW_SPEED_KP);
    _registry->internal_set<float>("control.thurst_offset", 0.26f);
    _registry->internal_set<float>("control.attitude.roll.speed", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.speed", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed_target", 0.0f);
    _registry->internal_set<float>("control.attitude.height.speed", 0.0f);
    _registry->internal_set<float>("control.attitude.height.speed_target", 0.0f);
    _registry->internal_set<float>("control.position.x.speed", 0.0f);
    _registry->internal_set<float>("control.position.x.speed_target", 0.0f);
    _registry->internal_set<float>("control.position.y.speed", 0.0f);
    _registry->internal_set<float>("control.position.y.speed_target", 0.0f);
    _registry->internal_set<float>("control.position.z.speed", 0.0f);
    _registry->internal_set<float>("control.position.z.speed_target", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.position", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.position", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.position", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.height.position", 0.0f);
    _registry->internal_set<float>("control.attitude.height.position_target", 0.0f);
    _registry->internal_set<float>("control.position.x.position", 0.0f);
    _registry->internal_set<float>("control.position.x.position_target", 0.0f);
    _registry->internal_set<float>("control.position.y.position", 0.0f);
    _registry->internal_set<float>("control.position.y.position_target", 0.0f);
    _registry->internal_set<float>("control.position.z.position", 0.0f);
    _registry->internal_set<float>("control.position.z.position_target", 0.0f);

    _height_controller = new Controller(period,
                                        new Pid(period, ATTITUDE_PID_HEIGHT_SPEED_KP, ATTITUDE_PID_HEIGHT_SPEED_KI, ATTITUDE_PID_HEIGHT_SPEED_KD),
                                        new Pid(period, ATTITUDE_PID_HEIGHT_POSITION_KP, ATTITUDE_PID_HEIGHT_POSITION_KI, ATTITUDE_PID_HEIGHT_POSITION_KD));

    _roll_controller   = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_ROLL_SPEED_KP, ATTITUDE_PID_ROLL_SPEED_KI, ATTITUDE_PID_ROLL_SPEED_KD),
                                        new Pid(period, ATTITUDE_PID_ROLL_POSITION_KP, ATTITUDE_PID_ROLL_POSITION_KI, ATTITUDE_PID_ROLL_POSITION_KD));

    _pitch_controller  = new Controller(period,
                                        new Pid(period, ATTITUDE_PID_PITCH_SPEED_KP, ATTITUDE_PID_PITCH_SPEED_KI, ATTITUDE_PID_PITCH_SPEED_KD),
                                        new Pid(period, ATTITUDE_PID_PITCH_POSITION_KP, ATTITUDE_PID_PITCH_POSITION_KI, ATTITUDE_PID_PITCH_POSITION_KD));

    _yaw_controller    = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_YAW_SPEED_KP, ATTITUDE_PID_YAW_SPEED_KI, ATTITUDE_PID_YAW_SPEED_KD),
                                        new Pid(period, ATTITUDE_PID_YAW_POSITION_KP, ATTITUDE_PID_YAW_POSITION_KI, ATTITUDE_PID_YAW_POSITION_KD));

    Motor * front_left  = new Motor(ATTITUDE_MOTOR_FRONT_LEFT_PULSE_CHANNEL,  ATTITUDE_MOTOR_FRONT_LEFT_PULSE_PIN);
    Motor * front_right = new Motor(ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_CHANNEL, ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_PIN);
    Motor * rear_left   = new Motor(ATTITUDE_MOTOR_REAR_LEFT_PULSE_CHANNEL,   ATTITUDE_MOTOR_REAR_LEFT_PULSE_PIN);
    Motor * rear_right  = new Motor(ATTITUDE_MOTOR_REAR_RIGHT_PULSE_CHANNEL,  ATTITUDE_MOTOR_REAR_RIGHT_PULSE_PIN);

    front_left->arm();
    front_right->arm();
    rear_left->arm();
    rear_right->arm();

    _mixer = new Mixer(front_left, front_right, rear_left, rear_right);

    LOG_INFO("Init done");
}

void AttitudeController::run(void)
{
    float  gx, gy, gz;           /* gyro in drone frame (sensor data) */
    float  ax, ay, az;           /* accelero in drone frame (sensor data) */
    float  ax_r, ay_r, az_r;     /* accelero in earth  frame */
    float  mx, my, mz;           /* magneto in drone frame (sensor data) */
    float  ultrasound = 0.0;
    float  barometer = 0.0;
    float  roll = 0.0, pitch = 0.0, yaw = 0.0;     /* previous values */

    /* Read the sensors */

    _marg->read_acc(&ax, &ay, &az);
    _marg->read_gyro(&gx, &gy, &gz);
    _marg->read_mag(&mx, &my, &mz);

    barometer  = _baro->height();
    ultrasound = _ultrasound->height();

    /* Estimate the attitude */

    roll  = _euler_observer->roll();
    pitch = _euler_observer->pitch();
    yaw   = _euler_observer->yaw();

    _euler_observer->update(gx, gy, gz, ax, ay, az, mx, my, mz);
    _euler_observer->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);

    _height_observer->update((az_r - 0.948) * 10.0, barometer, ultrasound);

    _roll_speed   = (_euler_observer->roll()  - roll)  / _period;
    _pitch_speed  = (_euler_observer->pitch() - pitch) / _period;
    _yaw_speed    = (_euler_observer->yaw()   - yaw)   / _period;
    _height_speed = _height_observer->vertical_speed();

    // printf("%f\n", ultrasound);
    // printf("%f\n", _barometer);
    // printf("%f;%f;%f;%f\n", az_r - 0.97f, _height_observer->vertical_speed(), _height_observer->height(), ultrasound);
    // printf("%f;%f;%f\n", 180.0 * _euler_observer->roll() / 3.1416, 180.0 * _euler_observer->pitch() / 3.1416, 180.0 * _euler_observer->yaw() / 3.1416);
    // printf("%f;%f;%f\n", ax_r, ay_r, az_r);

    if (_registry->internal_get<string>("control.mode") == "attitude")
    {
        float height_command, roll_command, pitch_command, yaw_command;

        if (_registry->internal_get<string>("control.attitude.height.mode") == "speed")
        {
            _height_controller->update_target(Controller::Mode::SPEED, _registry->internal_get<float>("control.attitude.height.speed_target"));
            _height_controller->update(_height_observer->height(), _height_observer->vertical_speed());
            height_command = _height_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.height.mode") == "position")
        {
            _height_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.height.position_target"));
            _height_controller->update(_height_observer->height(), _height_observer->vertical_speed());
            height_command = _height_controller->command();
        }
        else
        {
            height_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.roll.mode") == "position")
        {
            _roll_controller->update_target(Controller::POSITION, _registry->internal_get<float>("control.attitude.roll.position_target"));
            _roll_controller->update(_euler_observer->roll(), gx);
            roll_command = _roll_controller->command();
        }
        else
        {
            roll_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.pitch.mode") == "position")
        {
            _pitch_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.pitch.position_target"));
            _pitch_controller->update(_euler_observer->pitch(), gy);
            pitch_command = _pitch_controller->command();
        }
        else
        {
            pitch_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.yaw.mode") == "speed")
        {
            _yaw_controller->update_target(Controller::Mode::SPEED, _registry->internal_get<float>("control.attitude.yaw.speed_target"));
            _yaw_controller->update(_euler_observer->yaw(), gz);
            yaw_command = _yaw_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.yaw.mode") == "position")
        {
            _yaw_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.yaw.position_target"));
            _yaw_controller->update(_euler_observer->yaw(), gz);
            yaw_command = _yaw_controller->command();
        }
        else
        {
            yaw_command = 0.0f;
        }

        /* Apply the controls to the motors */
        _mixer->update(_registry->internal_get<float>("control.thurst_offset"), roll_command, pitch_command, yaw_command);
    }
    else if (_registry->internal_get<string>("control.mode") == "motors")
    {
        _mixer->set_motors_speeds(_registry->internal_get<float>("control.motors.front_left"),
                                  _registry->internal_get<float>("control.motors.front_right"),
                                  _registry->internal_get<float>("control.motors.rear_left"),
                                  _registry->internal_get<float>("control.motors.rear_right"));
    }
    else if (_registry->internal_get<string>("control.mode") == "off")
    {
        _mixer->update(0.0f, 0.0f, 0.0f, 0.0f);
    }
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
