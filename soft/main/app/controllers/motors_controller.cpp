#include <app/controllers/motors_controller.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

MotorsController::MotorsController(float period, DataRessourcesRegistry * registry, Mixer * mixer):
    PeriodicTask("motors_ctlr", Task::Priority::LOW, (int)(period * 1000), false)
{
    _period    = period;
    _registry  = registry;
    _mixer     = mixer;

    _registry->internal_set<float>("control.motors.front_left", 0.0);
    _registry->internal_set<float>("control.motors.front_right", 0.0);
    _registry->internal_set<float>("control.motors.rear_left", 0.0);
    _registry->internal_set<float>("control.motors.rear_right", 0.0);

    LOG_INFO("Init done");
}

void IRAM_ATTR MotorsController::run(void)
{
    /* Run the controllers */
    if (_registry->internal_get<string>("control.mode") == "motors")
    {
        _mixer->set_motors_speeds(_registry->internal_get<float>("control.motors.front_left"),
                                  _registry->internal_get<float>("control.motors.front_right"),
                                  _registry->internal_get<float>("control.motors.rear_left"),
                                  _registry->internal_get<float>("control.motors.rear_right"));
    }
    else if (_registry->internal_get<string>("control.mode") == "off")
    {
        _mixer->set_motors_speeds(0.0, 0.0, 0.0, 0.0);
    }
}
