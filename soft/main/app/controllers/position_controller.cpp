#include <app/controllers/position_controller.h>
#include <esp_attr.h>
#include <hal/log.h>
#include <os/task.h>

PositionController::PositionController(float period, DataRessourcesRegistry * registry):
    PeriodicTask("attitude_ctlr", Task::Priority::MEDIUM, (int)(period * 1000), false)
{
    _registry = registry;

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.x.position", 0.0f);
    _registry->internal_set<float>("control.position.x.position_target", 0.0f);
    _registry->internal_set<float>("control.position.x.speed", 0.0f);
    _registry->internal_set<float>("control.position.x.speed_target", 0.0f);

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.y.position", 0.0f);
    _registry->internal_set<float>("control.position.y.position_target", 0.0f);
    _registry->internal_set<float>("control.position.y.speed", 0.0f);
    _registry->internal_set<float>("control.position.y.speed_target", 0.0f);

    _registry->internal_set<string>("control.position.x.mode", "off");
    _registry->internal_set<float>("control.position.z.position", 0.0f);
    _registry->internal_set<float>("control.position.z.position_target", 0.0f);
    _registry->internal_set<float>("control.position.z.speed", 0.0f);
    _registry->internal_set<float>("control.position.z.speed_target", 0.0f);

    LOG_INFO("Init done");
}

void PositionController::run(void)
{
    return;
}
