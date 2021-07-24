#include <app/workers/battery_supervisor.h>
#include <hal/log.h>

BatterySupervisor::BatterySupervisor(float period, DataRessourcesRegistry * registry):
    PeriodicTask("battery_supervisor", configMAX_PRIORITIES - 5, (int)(period * 1000), false)
{
    _registry = registry;

    registry->internal_set<int>("battery.level", 100);

    LOG_INFO("Init done");
}

void BatterySupervisor::run(void)
{
    return;
}
