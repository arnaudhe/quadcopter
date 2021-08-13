#include <app/workers/battery_supervisor.h>
#include <hal/log.h>

BatterySupervisor::BatterySupervisor(float period, Battery * battery, DataRessourcesRegistry * registry):
    PeriodicTask("battery_supervisor", Task::Priority::VERY_LOW, (int)(period * 1000), false)
{
    _registry = registry;
    _battery  = battery;

    _registry->internal_set<int>("battery.level", 100);

    LOG_INFO("Init done");
}

void BatterySupervisor::run(void)
{
    float level;

    level = _battery->get_level();
    LOG_INFO("Battery percent = %.1f %", level);
    _registry->internal_set<int>("battery.level", (int)level);
}
