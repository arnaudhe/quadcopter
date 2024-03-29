#include <app/controllers/height_controller.h>
#include <app/controllers/height_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

#define GRAVITATIONAL_ACCELERATION      9.80665

HeightController::HeightController(float period, DataRessourcesRegistry * registry, Marg * marg, Barometer * baro, UltrasoundSensor * ultrasound, AttitudeController * attitude_controller, RateController * rate_controller):
    PeriodicTask("height_ctlr", Task::Priority::HIGH, 16 * 1024, (int)(period * 1000), false)
{
    _period              = period;
    _registry            = registry;
    _marg                = marg;
    _baro                = baro;
    _ultrasound          = ultrasound;
    _attitude_controller = attitude_controller;
    _rate_controller     = rate_controller;

    _observer = new HeightObserver(period);

    _registry->internal_set<string>("control.attitude.height.mode", "speed");

    _registry->internal_set<float>("control.attitude.height.manual.throttle", 0.4f);

    _registry->internal_set<float>("control.attitude.height.position.current", 0.0f);
    _registry->internal_set<float>("control.attitude.height.position.target", 0.2f);
    _registry->internal_set<float>("control.attitude.height.position.kp", HEIGHT_PID_POSITION_KP);
    _registry->internal_set<float>("control.attitude.height.position.ki", HEIGHT_PID_POSITION_KI);
    _registry->internal_set<float>("control.attitude.height.position.kd", HEIGHT_PID_POSITION_KD);
    _registry->internal_set<float>("control.attitude.height.position.kff", 0.0f);

    _registry->internal_set<float>("control.attitude.height.speed.current", 0.0f);
    _registry->internal_set<float>("control.attitude.height.speed.target", 0.0f);
    _registry->internal_set<float>("control.attitude.height.speed.kp", HEIGHT_PID_SPEED_KP);
    _registry->internal_set<float>("control.attitude.height.speed.ki", HEIGHT_PID_SPEED_KI);
    _registry->internal_set<float>("control.attitude.height.speed.kd", HEIGHT_PID_SPEED_KD);
    _registry->internal_set<float>("control.attitude.height.speed.kff", 0.0f);

    _position_controller = new Pid(period, HEIGHT_PID_POSITION_KP, HEIGHT_PID_POSITION_KI, HEIGHT_PID_POSITION_KD, 0.0f, 0.0f, -0.12f, 0.12f);
    _speed_controller    = new Pid(period, HEIGHT_PID_SPEED_KP, HEIGHT_PID_SPEED_KI, HEIGHT_PID_SPEED_KD, 0.0f, 0.0f, -0.2f, 0.2f);

    LOG_INFO("Init done");
}

void HeightController::run(void)
{
    float ax, ay, az;           /* accelero in drone frame (sensor data) */
    float ax_r, ay_r, az_r;     /* accelero in earth frame */
    float ultrasound = 0.0;
    float barometer  = 0.0;
    float vertical_acceleration;

    /* Read the sensors */
    _rate_controller->get_acc(&ax, &ay, &az);
    barometer  = _baro->height();
    ultrasound = _ultrasound->height();

    /* Rotate acceleration vector in drone frame to acceleration in earth frame */
    _attitude_controller->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);

    /* Compute the vertical acceleration by removing constant 1g gravity acceleration and convert to m.s^2 */
    vertical_acceleration = (az_r - 1.0) * GRAVITATIONAL_ACCELERATION;

    /* Update the Kalman filter to compute current estimated height */
    _observer->update(vertical_acceleration, barometer, ultrasound);

    _registry->internal_set<float>("sensors.ultrasound.height", ultrasound);
    _registry->internal_set<float>("sensors.barometer.height", barometer);
    _registry->internal_set<float>("sensors.accelerometer.vertical", vertical_acceleration);
    _registry->internal_set<float>("control.attitude.height.position.current", _observer->height());
    _registry->internal_set<float>("control.attitude.height.speed.current", _observer->vertical_speed());

    /* Run the controllers */
    if (_registry->internal_get<string>("control.mode") == "attitude")
    {
        float height_command;

        _position_controller->set_kp(_registry->internal_get<float>("control.attitude.height.position.kp"));
        _position_controller->set_ki(_registry->internal_get<float>("control.attitude.height.position.ki"));
        _position_controller->set_kd(_registry->internal_get<float>("control.attitude.height.position.kd"));
        _position_controller->set_kff(_registry->internal_get<float>("control.attitude.height.position.kff"));

        _speed_controller->set_kp(_registry->internal_get<float>("control.attitude.height.speed.kp"));
        _speed_controller->set_ki(_registry->internal_get<float>("control.attitude.height.speed.ki"));
        _speed_controller->set_kd(_registry->internal_get<float>("control.attitude.height.speed.kd"));
        _speed_controller->set_kff(_registry->internal_get<float>("control.attitude.height.speed.kff"));

        if (_registry->internal_get<string>("control.attitude.height.mode") == "speed")
        {
            _speed_controller->set_setpoint(_registry->internal_get<float>("control.attitude.height.speed.target"));
            height_command = HEIGHT_THURST_OFFSET + _speed_controller->update(_observer->vertical_speed());
            _registry->internal_set<float>("control.attitude.height.speed.command", height_command);
            _position_controller->reset();
        }
        else if (_registry->internal_get<string>("control.attitude.height.mode") == "position")
        {
            height_command =  HEIGHT_THURST_OFFSET + _position_controller->update(ultrasound);
            _position_controller->set_setpoint(_registry->internal_get<float>("control.attitude.height.position.target"));
            _registry->internal_set<float>("control.attitude.height.position.command", height_command);
            _speed_controller->reset();
        }
        else
        {
            _position_controller->reset();
            _speed_controller->reset();
            height_command = _registry->internal_get<float>("control.attitude.height.manual.throttle");
        }

        _rate_controller->set_throttle(height_command);
    }
    else
    {
        _position_controller->reset();
        _speed_controller->reset();
        _rate_controller->set_throttle(0.0f);
    }
}
