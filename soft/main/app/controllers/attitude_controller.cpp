#include <app/controllers/attitude_controller.h>
#include <app/controllers/attitude_controller_conf.h>
#include <esp_log.h>

AttitudeController::AttitudeController(float period, DataRessourcesRegistry * registry)
{
    _period             = period;
    _barometer_timing   = 0.0;
    _barometer_waiting  = false;
    _registry           = registry;

    _euler_observer = new EulerObserver(period);
    _height_observer = new HeightObserver(period);

    _i2c        = new I2cMaster(I2C_MASTER_NUM);
    _marg       = new Marg(_i2c);
    _baro       = new Barometer(_i2c);
    _ultrasound = new UltrasoundSensor();

    _i2c->init();
    _marg->init();
    _baro->init();

    _baro->read(_barometer);

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

    ESP_LOGI("attitude_controller", "Init done");
}

void AttitudeController::update(void)
{
    float  gx, gy, gz;           /* gyro in drone frame (sensor data) */
    float  ax, ay, az;           /* accelero in drone frame (sensor data) */
    float  ax_r, ay_r, az_r;     /* accelero in earth frame */
    float  mx, my, mz;           /* magneto in drone frame (sensor data) */
    double temp;                 /* barometer temperature sensor data */
    double ultrasound = 0.0;
    float  roll, pitch, yaw;     /* previous values */

    /* Read the sensors */

    _marg->read_acc(&ax, &ay, &az);
    _marg->read_gyro(&gx, &gy, &gz);
    _marg->read_mag(&mx, &my, &mz);

    if (_barometer_timing >= (0.031))
    {
        _baro->read(_barometer);
        _barometer_timing = 0.0;
        _barometer_waiting = false;
    }
    else if ((_barometer_timing >= 0.005) && (_barometer_waiting == false))
    {
        _barometer_waiting = true;
        _baro->read_temperature(temp);
        // printf("temperature %f\n", temp);
    }
    _barometer_timing += _period;

    ultrasound = _ultrasound->height();

    /* Estimate the attitude */

    roll  = _euler_observer->roll();
    pitch = _euler_observer->pitch();
    yaw   = _euler_observer->yaw();

    _euler_observer->update(gx, gy, gz, ax, ay, az, mx, my, mz);
    _euler_observer->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);
    _height_observer->update(az_r - 0.97f, _barometer, ultrasound);

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
            _height_controller->update (_height_observer->height(), _height_observer->vertical_speed());
            height_command = _height_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.height.mode") == "position")
        {
            _height_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.height.position_target"));
            _height_controller->update (_height_observer->height(), _height_observer->vertical_speed());
            height_command = _height_controller->command();
        }
        else
        {
            height_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.roll.mode") == "speed")
        {
            _roll_controller->update_target(Controller::Mode::SPEED, _registry->internal_get<float>("control.attitude.roll.speed_target"));
            _roll_controller->update (_euler_observer->roll(), _roll_speed);
            roll_command = _roll_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.roll.mode") == "position")
        {
            _roll_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.roll.position_target"));
            _roll_controller->update (_euler_observer->roll(), _roll_speed);
            roll_command = _roll_controller->command();
        }
        else
        {
            roll_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.pitch.mode") == "speed")
        {
            _pitch_controller->update_target(Controller::Mode::SPEED, _registry->internal_get<float>("control.attitude.pitch.speed_target"));
            _pitch_controller->update (_euler_observer->pitch(), _pitch_speed);
            pitch_command = _pitch_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.pitch.mode") == "position")
        {
            _pitch_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.pitch.position_target"));
            _pitch_controller->update (_euler_observer->pitch(), _pitch_speed);
            pitch_command = _pitch_controller->command();
        }
        else
        {
            pitch_command = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.yaw.mode") == "speed")
        {
            _yaw_controller->update_target(Controller::Mode::SPEED, _registry->internal_get<float>("control.attitude.yaw.speed_target"));
            _yaw_controller->update (_euler_observer->yaw(), _yaw_speed);
            yaw_command = _yaw_controller->command();
        }
        else if (_registry->internal_get<string>("control.attitude.yaw.mode") == "position")
        {
            _yaw_controller->update_target(Controller::Mode::POSITION, _registry->internal_get<float>("control.attitude.yaw.position_target"));
            _yaw_controller->update (_euler_observer->yaw(), _yaw_speed);
            yaw_command = _yaw_controller->command();
        }
        else
        {
            yaw_command = 0.0f;
        }

        /* Apply the controls to the motors */
        _mixer->update(height_command, roll_command, pitch_command, yaw_command);
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
