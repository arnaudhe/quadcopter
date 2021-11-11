#pragma once

#include <utils/mixer.h>

#include <os/periodic_task.h>
#include <os/mutex.h>

#include <data_model/data_ressources_registry.h>

class MotorsController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    Mixer                   * _mixer;
    float                     _period;

    void run(void);

public:

    MotorsController(float period, DataRessourcesRegistry * registry, Mixer * mixer);
};