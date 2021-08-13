#pragma once

#include <string>

#include <os/periodic_task.h>

#include <hal/battery.h>

#include <data_model/data_ressources_registry.h>

class BatterySupervisor : public PeriodicTask
{

  private:

    DataRessourcesRegistry  * _registry;
    Battery                 * _battery;

    void run();

  public:

    BatterySupervisor(float period, Battery * battery, DataRessourcesRegistry * registry);
};