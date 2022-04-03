#include <app/controllers/position_controller.h>
#include <esp_attr.h>
#include <hal/log.h>
#include <os/task.h>

PositionController::PositionController(float period, DataRessourcesRegistry * registry, Gps * gps, AttitudeController * attitude_controller, RateController * rate_controller):
    PeriodicTask("attitude_ctlr", Task::Priority::MEDIUM, 12 * 1024, (int)(period * 1000), false)
{
    _registry            = registry;
    _gps                 = gps;
    _attitude_controller = attitude_controller;
    _rate_controller     = rate_controller;

    _observer = new PositionObserver(period);

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.x.position.current", 0.0f);
    _registry->internal_set<float>("control.position.x.position.target", 0.0f);
    _registry->internal_set<float>("control.position.x.speed.current", 0.0f);
    _registry->internal_set<float>("control.position.x.speed.target", 0.0f);

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.y.position.current", 0.0f);
    _registry->internal_set<float>("control.position.y.position.target", 0.0f);
    _registry->internal_set<float>("control.position.y.speed.current", 0.0f);
    _registry->internal_set<float>("control.position.y.speed.target", 0.0f);

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.z.position.current", 0.0f);
    _registry->internal_set<float>("control.position.z.position.target", 0.0f);
    _registry->internal_set<float>("control.position.z.speed.current", 0.0f);
    _registry->internal_set<float>("control.position.z.speed.target", 0.0f);

    LOG_INFO("Init done");
}

void PositionController::run(void)
{
    float ax, ay, az;           /* accelero in drone frame (sensor data) */
    float ax_r, ay_r, az_r;     /* accelero in earth frame */

    /* Read the sensors */
    _rate_controller->get_acc(&ax, &ay, &az);

    /* Rotate acceleration vector in drone frame to acceleration in earth frame */
    _attitude_controller->rotate(ax, ay, az, &ax_r, &ay_r, &az_r);

     /* Update the Kalman filter to compute current estimated height */
    _observer->update(ax_r, ay_r, 0.0f, 0.0f);
}
