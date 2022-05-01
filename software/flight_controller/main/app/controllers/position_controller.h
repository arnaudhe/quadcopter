#pragma once

#include <utils/pid.h>
#include <utils/kalman.h>

#include <hal/marg.h>
#include <hal/barometer.h>
#include <hal/ultrasound.h>

#include <os/periodic_task.h>

#include <app/controllers/height_controller.h>
#include <app/controllers/attitude_controller.h>

#include <data_model/data_ressources_registry.h>

class PositionController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    float                     _period;

    void run(void);

public:

    PositionController(float period, DataRessourcesRegistry * registry);
};