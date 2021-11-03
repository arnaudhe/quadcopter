#include <app/controllers/height_controller.h>
#include <app/controllers/height_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

HeightController::HeightController(float period, DataRessourcesRegistry * registry, Marg * marg, Barometer * baro, UltrasoundSensor * ultrasound, AttitudeController * attitude_controller, RateController * rate_controller):
    PeriodicTask("height_ctlr", Task::Priority::HIGH, (int)(period * 1000), false)
{
    _period              = period;
    _registry            = registry;
    _marg                = marg;
    _baro                = baro;
    _ultrasound          = ultrasound;
    _attitude_controller = attitude_controller;
    _rate_controller     = rate_controller;

    _observer = new HeightObserver(period);

    _registry->internal_set<float>("control.thurst_offset", 0.26f);
    _registry->internal_set<string>("control.attitude.height.mode", "off");
    _registry->internal_set<float>("control.attitude.height.speed", 0.0f);
    _registry->internal_set<float>("control.attitude.height.speed_target", 0.0f);
    _registry->internal_set<float>("control.attitude.height.position", 0.0f);
    _registry->internal_set<float>("control.attitude.height.position_target", 0.0f);

    _position_controller = new Pid(period, HEIGHT_PID_POSITION_KP, HEIGHT_PID_POSITION_KI, HEIGHT_PID_POSITION_KD);
    _speed_controller    = new Pid(period, HEIGHT_PID_SPEED_KP, HEIGHT_PID_SPEED_KI, HEIGHT_PID_SPEED_KD);

    LOG_INFO("Init done");
}

void IRAM_ATTR HeightController::run(void)
{
    float ax, ay, az;           /* accelero in drone frame (sensor data) */
    float ax_r, ay_r, az_r;     /* accelero in earth frame */
    float ultrasound = 0.0;
    float barometer  = 0.0;

    /* Read the sensors */
    _marg->read_acc(&ax, &ay, &az);
    barometer  = _baro->height();
    ultrasound = _ultrasound->height();

    /* Estimate the attitude */
    _attitude_controller->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);
    _observer->update((az_r - 0.948) * 10.0, barometer, ultrasound);

    /* Run the controllers */
    if (_registry->internal_get<string>("control.mode") == "attitude")
    {
        float height_command;

        if (_registry->internal_get<string>("control.attitude.height.mode") == "speed")
        {
            _speed_controller->set_setpoint(_registry->internal_get<float>("control.attitude.height.speed_target"));
            height_command = _speed_controller->update(_observer->vertical_speed());
        }
        else if (_registry->internal_get<string>("control.attitude.height.mode") == "position")
        {
            _position_controller->set_setpoint(_registry->internal_get<float>("control.attitude.height.position_target"));
            height_command = _position_controller->update(_observer->height());
        }
        else
        {
            height_command = _registry->internal_get<float>("control.thurst_offset");
        }

        _rate_controller->set_throttle(height_command);
    }
    else
    {
        _rate_controller->set_throttle(0.0f);
    }
}
