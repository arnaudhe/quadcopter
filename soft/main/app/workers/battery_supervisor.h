#pragma once

#include <string>

#include <os/periodic_task.h>

#include <data_model/data_ressources_registry.h>

class BatterySupervisor : public PeriodicTask
{

  private:

    DataRessourcesRegistry  * _registry;

    void run();

  public:

    BatterySupervisor(float period, DataRessourcesRegistry * registry);
};