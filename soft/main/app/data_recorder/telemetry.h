#pragma once

#include <os/task.h>
#include <data_model/data_ressources_registry.h>

class Telemetry : public Task
{

private:

    DataRessourcesRegistry * _registry;

    void run(void);

public:

    Telemetry(DataRessourcesRegistry * registy, int period);

};