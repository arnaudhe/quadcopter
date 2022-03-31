#pragma once

#include <data_model/data_ressources_registry.h>
#include <os/task.h>
#include <app/workers/broker.h>

class Telemetry : public Task
{

private:

    DataRessourcesRegistry * _registry;
    Broker                 * _broker;

    void run(void);

public:

    Telemetry(DataRessourcesRegistry * registy, int period, Broker * broker);

};