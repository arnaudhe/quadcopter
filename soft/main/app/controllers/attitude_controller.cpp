#include <app/controllers/attitude_controller.h>
#include <app/controllers/attitude_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

AttitudeController::AttitudeController(float period, DataRessourcesRegistry * registry, RateController * rate_controller, Marg * marg):
    PeriodicTask("attitude_ctlr", configMAX_PRIORITIES - 3, (int)(period * 1000), false)
{
    _period          = period;
    _registry        = registry;
    _rate_controller = rate_controller;
    _marg            = marg;
    _mutex           = new Mutex();
    _observer        = new EulerObserver(period);

    _registry->internal_set<string>("control.attitude.roll.mode", "off");
    _registry->internal_set<float>("control.attitude.roll.position", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.roll.speed", 0.0f);

    _registry->internal_set<float>("control.attitude.roll.kp", ATTITUDE_PID_ROLL_POSITION_KP);
    _registry->internal_set<float>("control.attitude.roll.ki", ATTITUDE_PID_ROLL_POSITION_KI);
    _registry->internal_set<float>("control.attitude.roll.kd", ATTITUDE_PID_ROLL_POSITION_KD);

    _registry->internal_set<string>("control.attitude.pitch.mode", "off");
    _registry->internal_set<float>("control.attitude.pitch.position", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.pitch.speed", 0.0f);

    _registry->internal_set<string>("control.attitude.yaw.mode", "off");
    _registry->internal_set<float>("control.attitude.yaw.position", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.position_target", 0.0f);
    _registry->internal_set<float>("control.attitude.yaw.speed", 0.0f);

    _roll_controller  = new Pid(period, ATTITUDE_PID_ROLL_POSITION_KP, ATTITUDE_PID_ROLL_POSITION_KI, ATTITUDE_PID_ROLL_POSITION_KD);
    _pitch_controller = new Pid(period, ATTITUDE_PID_PITCH_POSITION_KP, ATTITUDE_PID_PITCH_POSITION_KI, ATTITUDE_PID_PITCH_POSITION_KD);
    _yaw_controller   = new Pid(period, ATTITUDE_PID_YAW_POSITION_KP, ATTITUDE_PID_YAW_POSITION_KI, ATTITUDE_PID_YAW_POSITION_KD);

    LOG_INFO("Init done");
}

void IRAM_ATTR AttitudeController::run(void)
{
    float  gx, gy, gz;     /* gyro in drone frame (sensor data) */
    float  ax, ay, az;     /* accelero in drone frame (sensor data) */
    float  mx, my, mz;     /* magneto in drone frame (sensor data) */
    float  roll_rate_setpoint, pitch_rate_setpoint, yaw_rate_setpoint;
    bool   roll_enable, pitch_enable, yaw_enable;
    float  roll, pitch, yaw;

    /* Read the sensors */
    _rate_controller->get_rates(&gx, &gy, &gz); // Get the filtered rates from rate_controller
    _marg->read_acc(&ax, &ay, &az);
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
        if (_registry->internal_get<string>("control.attitude.roll.mode") == "position")
        {
            _roll_controller->set_kp(_registry->internal_get<float>("control.attitude.roll.kp"));
            _roll_controller->set_ki(_registry->internal_get<float>("control.attitude.roll.ki"));
            _roll_controller->set_kd(_registry->internal_get<float>("control.attitude.roll.kd"));

            _roll_controller->set_setpoint(_registry->internal_get<float>("control.attitude.roll.position_target"));

            roll_enable        = true;
            roll_rate_setpoint = _roll_controller->update(roll);
        }
        else if (_registry->internal_get<string>("control.attitude.roll.mode") == "speed")
        {
            roll_enable        = true;
            roll_rate_setpoint = _registry->internal_get<float>("control.attitude.roll.speed_target");
        }
        else
        {
            roll_enable        = false;
            roll_rate_setpoint = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.pitch.mode") == "position")
        {
            _pitch_controller->set_setpoint(_registry->internal_get<float>("control.attitude.pitch.position_target"));

            pitch_enable        = true;
            pitch_rate_setpoint = _pitch_controller->update(pitch);
        }
        else if (_registry->internal_get<string>("control.attitude.pitch.mode") == "speed")
        {
            pitch_enable        = true;
            pitch_rate_setpoint = _registry->internal_get<float>("control.attitude.pitch.speed_target");
        }
        else
        {
            pitch_enable        = false;
            pitch_rate_setpoint = 0.0f;
        }

        if (_registry->internal_get<string>("control.attitude.yaw.mode") == "position")
        {
            _yaw_controller->set_setpoint(_registry->internal_get<float>("control.attitude.yaw.position_target"));

            yaw_enable        = true;
            yaw_rate_setpoint = _yaw_controller->update(yaw);
        }
        else if (_registry->internal_get<string>("control.attitude.yaw.mode") == "speed")
        {
            yaw_enable        = true;
            yaw_rate_setpoint = _registry->internal_get<float>("control.attitude.yaw.speed_target");
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
}

void AttitudeController::rotate(float x, float y, float z, float * x_r, float * y_r, float * z_r)
{
    _mutex->lock();
    _observer->rotate(x, y, z, x_r, y_r, z_r);
    _mutex->unlock();
}
