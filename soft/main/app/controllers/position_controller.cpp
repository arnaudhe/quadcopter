#include <app/controllers/position_controller.h>
#include <esp_attr.h>
#include <hal/log.h>
#include <os/task.h>

PositionController::PositionController(float period, DataRessourcesRegistry * registry):
    PeriodicTask("attitude_ctlr", Task::Priority::MEDIUM, 16 * 1024, (int)(period * 1000), false)
{
    _registry = registry;

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
    return;
}
