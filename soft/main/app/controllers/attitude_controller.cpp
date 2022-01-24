#include <app/controllers/attitude_controller.h>
#include <app/controllers/attitude_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

AttitudeController::AttitudeController(float period, DataRessourcesRegistry * registry, RateController * rate_controller, Marg * marg):
    PeriodicTask("attitude_ctlr", Task::Priority::HIGH, (int)(period * 1000), false)
{
    _period          = period;
    _registry        = registry;
    _rate_controller = rate_controller;
    _marg            = marg;
    _mutex           = new Mutex();
    _observer        = new EulerObserver(period);

    _registry->internal_set<string>("control.attitude.roll.mode", "position");
    _registry->internal_set<float>("control.attitude.roll.position.current", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.position.target", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.position.command", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.speed.current", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.speed.target", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.speed.command", 0.0f);

    _registry->internal_set<float>("control.attitude.roll.position.kp", ATTITUDE_PID_ROLL_POSITION_KP);
    _registry->internal_set<float>("control.attitude.roll.position.ki", ATTITUDE_PID_ROLL_POSITION_KI);
    _registry->internal_set<float>("control.attitude.roll.position.kd", ATTITUDE_PID_ROLL_POSITION_KD);
    _registry->internal_set<float>("control.attitude.roll.position.kff", ATTITUDE_PID_ROLL_POSITION_FF);
    _registry->internal_set<float>("control.attitude.roll.position.kt", ATTITUDE_PID_ROLL_POSITION_AW);

    _registry->internal_set<string>("control.attitude.pitch.mode", "position");
    _registry->internal_set<float>("control.attitude.pitch.position.current", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.position.target", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.position.command", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.speed.current", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.speed.target", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.speed.command", 0.0f);

    _registry->internal_set<float>("control.attitude.pitch.position.kp", ATTITUDE_PID_PITCH_POSITION_KP);
    _registry->internal_set<float>("control.attitude.pitch.position.ki", ATTITUDE_PID_PITCH_POSITION_KI);
    _registry->internal_set<float>("control.attitude.pitch.position.kd", ATTITUDE_PID_PITCH_POSITION_KD);
    _registry->internal_set<float>("control.attitude.pitch.position.kff", ATTITUDE_PID_PITCH_POSITION_FF);
    _registry->internal_set<float>("control.attitude.pitch.position.kt", ATTITUDE_PID_PITCH_POSITION_AW);

    _registry->internal_set<string>("control.attitude.yaw.mode", "speed");
    _registry->internal_set<float>("control.attitude.yaw.position.current", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.position.target", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.position.command", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed.current", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed.target", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed.command", 0.0f);

    _registry->internal_set<float>("control.attitude.yaw.position.kp", ATTITUDE_PID_YAW_POSITION_KP);
    _registry->internal_set<float>("control.attitude.yaw.position.ki", ATTITUDE_PID_YAW_POSITION_KI);
    _registry->internal_set<float>("control.attitude.yaw.position.kd", ATTITUDE_PID_YAW_POSITION_KD);
    _registry->internal_set<float>("control.attitude.yaw.position.kff", ATTITUDE_PID_YAW_POSITION_FF);
    _registry->internal_set<float>("control.attitude.yaw.position.kt", ATTITUDE_PID_YAW_POSITION_AW);

    _roll_controller  = new Pid(period, ATTITUDE_PID_ROLL_POSITION_KP, ATTITUDE_PID_ROLL_POSITION_KI, ATTITUDE_PID_ROLL_POSITION_KD,
                                        ATTITUDE_PID_ROLL_POSITION_FF, ATTITUDE_PID_ROLL_POSITION_AW);
    _pitch_controller = new Pid(period, ATTITUDE_PID_PITCH_POSITION_KP, ATTITUDE_PID_PITCH_POSITION_KI, ATTITUDE_PID_PITCH_POSITION_KD,
                                        ATTITUDE_PID_PITCH_POSITION_FF, ATTITUDE_PID_PITCH_POSITION_AW);
    _yaw_controller   = new Pid(period, ATTITUDE_PID_YAW_POSITION_KP, ATTITUDE_PID_YAW_POSITION_KI, ATTITUDE_PID_YAW_POSITION_KD,
                                        ATTITUDE_PID_YAW_POSITION_FF, ATTITUDE_PID_YAW_POSITION_AW);

    LOG_INFO("Init done");
}

void IRAM_ATTR AttitudeController::run(void)
{
    float  gx, gy, gz;     /* gyro in drone frame (sensor data) */
    float  ax, ay, az;     /* accelero in drone frame (sensor data) */
    float  mx, my, mz;     /* magneto in drone frame (sensor data) */
    float  roll_rate_setpoint = 0.0f, pitch_rate_setpoint = 0.0f, yaw_rate_setpoint = 0.0f;
    bool   roll_enable, pitch_enable, yaw_enable;
    float  roll, pitch, yaw;

    /* Read the sensors */
    _rate_controller->get_rates(&gx, &gy, &gz); // Get the filtered rates from rate controller
    _rate_controller->get_acc(&ax, &ay, &az); // Get the filtered acc from the rate controller
    _marg->read_mag(&mx, &my, &mz);

    /* Estimate the attitude */
    _mutex->lock();
    _observer->update(gx, gy, gz, ax, ay, az, mx, my, mz);
    roll  = _observer->roll();
    pitch = _observer->pitch();
    yaw   = _observer->yaw();
    _mutex->unlock();

    /* Run the controllers */
    if (_registry->internal_get<string>("control.mode") == "attitude")
    {
        _rate_controller->set_roll_pid(_registry->internal_get<float>("control.attitude.roll.speed.kp"),
                                       _registry->internal_get<float>("control.attitude.roll.speed.ki"),
                                       _registry->internal_get<float>("control.attitude.roll.speed.kd"),
                                       _registry->internal_get<float>("control.attitude.roll.speed.kff"),
                                       _registry->internal_get<float>("control.attitude.roll.speed.kt"));

        _roll_controller->set_kd(_registry->internal_get<float>("control.attitude.roll.position.kp"));
        _roll_controller->set_ki(_registry->internal_get<float>("control.attitude.roll.position.ki"));
        _roll_controller->set_kd(_registry->internal_get<float>("control.attitude.roll.position.kd"));
        _roll_controller->set_kff(_registry->internal_get<float>("control.attitude.roll.position.kff"));
        _roll_controller->set_kt(_registry->internal_get<float>("control.attitude.roll.position.kt"));

        if (_registry->internal_get<string>("control.attitude.roll.mode") == "position")
        {
            _roll_controller->set_setpoint(_registry->internal_get<float>("control.attitude.roll.position.target"));

            roll_enable        = true;
            roll_rate_setpoint = _roll_controller->update(roll);
        }
        else if (_registry->internal_get<string>("control.attitude.roll.mode") == "speed")
        {
            roll_enable        = true;
            roll_rate_setpoint = _registry->internal_get<float>("control.attitude.roll.speed.target");
        }
        else
        {
            roll_enable        = false;
            roll_rate_setpoint = 0.0f;
        }

        _rate_controller->set_pitch_pid(_registry->internal_get<float>("control.attitude.pitch.speed.kp"),
                                        _registry->internal_get<float>("control.attitude.pitch.speed.ki"),
                                        _registry->internal_get<float>("control.attitude.pitch.speed.kd"),
                                        _registry->internal_get<float>("control.attitude.pitch.speed.kff"),
                                        _registry->internal_get<float>("control.attitude.pitch.speed.kt"));

        _pitch_controller->set_kd(_registry->internal_get<float>("control.attitude.pitch.position.kp"));
        _pitch_controller->set_ki(_registry->internal_get<float>("control.attitude.pitch.position.ki"));
        _pitch_controller->set_kd(_registry->internal_get<float>("control.attitude.pitch.position.kd"));
        _pitch_controller->set_kff(_registry->internal_get<float>("control.attitude.pitch.position.kff"));
        _pitch_controller->set_kt(_registry->internal_get<float>("control.attitude.pitch.position.kt"));

        if (_registry->internal_get<string>("control.attitude.pitch.mode") == "position")
        {
            _pitch_controller->set_setpoint(_registry->internal_get<float>("control.attitude.pitch.position.target"));

            pitch_enable        = true;
            pitch_rate_setpoint = _pitch_controller->update(pitch);
        }
        else if (_registry->internal_get<string>("control.attitude.pitch.mode") == "speed")
        {
            pitch_enable        = true;
            pitch_rate_setpoint = _registry->internal_get<float>("control.attitude.pitch.speed.target");
        }
        else
        {
            pitch_enable        = false;
            pitch_rate_setpoint = 0.0f;
        }

        _rate_controller->set_yaw_pid(_registry->internal_get<float>("control.attitude.yaw.speed.kp"),
                                      _registry->internal_get<float>("control.attitude.yaw.speed.ki"),
                                      _registry->internal_get<float>("control.attitude.yaw.speed.kd"),
                                      _registry->internal_get<float>("control.attitude.yaw.speed.kff"),
                                      _registry->internal_get<float>("control.attitude.yaw.speed.kt"));

        _yaw_controller->set_kd(_registry->internal_get<float>("control.attitude.yaw.position.kp"));
        _yaw_controller->set_ki(_registry->internal_get<float>("control.attitude.yaw.position.ki"));
        _yaw_controller->set_kd(_registry->internal_get<float>("control.attitude.yaw.position.kd"));
        _yaw_controller->set_kff(_registry->internal_get<float>("control.attitude.yaw.position.kff"));
        _yaw_controller->set_kt(_registry->internal_get<float>("control.attitude.yaw.position.kt"));

        if (_registry->internal_get<string>("control.attitude.yaw.mode") == "position")
        {
            _yaw_controller->set_setpoint(_registry->internal_get<float>("control.attitude.yaw.position.target"));

            yaw_enable        = true;
            yaw_rate_setpoint = _yaw_controller->update(yaw);
        }
        else if (_registry->internal_get<string>("control.attitude.yaw.mode") == "speed")
        {
            yaw_enable        = true;
            yaw_rate_setpoint = _registry->internal_get<float>("control.attitude.yaw.speed.target");
        }
        else
        {
            yaw_enable        = false;
            yaw_rate_setpoint = 0.0f;
        }

        /* Apply the controls to the motors */
        _rate_controller->set_enables(roll_enable, pitch_enable, yaw_enable);
        _rate_controller->set_rate_targets(roll_rate_setpoint, pitch_rate_setpoint, yaw_rate_setpoint);
    }
    else
    {
        _rate_controller->set_enables(false, false, false);
    }

    _registry->internal_set<float>("control.attitude.roll.position.current", roll);
    _registry->internal_set<float>("control.attitude.roll.position.command", roll_rate_setpoint);

    _registry->internal_set<float>("control.attitude.pitch.position.current", pitch);
    _registry->internal_set<float>("control.attitude.pitch.position.command", pitch_rate_setpoint);

    _registry->internal_set<float>("control.attitude.yaw.position.current", yaw);
    _registry->internal_set<float>("control.attitude.yaw.position.command", yaw_rate_setpoint);
}

void AttitudeController::rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r)
{
    _mutex->lock();
    _observer->rotate(x, y, z, x_r, y_r, z_r);
    _mutex->unlock();
}
