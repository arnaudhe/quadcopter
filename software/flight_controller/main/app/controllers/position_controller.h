#pragma once

#include <utils/pid.h>
#include <utils/kalman.h>

#include <hal/gps.h>

#include <os/periodic_task.h>

#include <app/controllers/rate_controller.h>
#include <app/controllers/attitude_controller.h>
#include <app/observers/position_observer.h>

#include <data_model/data_ressources_registry.h>

class PositionController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    float                     _period;
    PositionObserver        * _observer;
    Gps                     * _gps;
    AttitudeController      * _attitude_controller;
    RateController          * _rate_controller;

    void run(void);

public:

    PositionController(float period, DataRessourcesRegistry * registry, Gps * gps, AttitudeController * attitude_controller, RateController * rate_controller);
};
