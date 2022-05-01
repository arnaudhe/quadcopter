#pragma once

#include <hal/marg.h>
#include <hal/barometer.h>
#include <hal/ultrasound.h>

#include <os/periodic_task.h>

#include <app/observers/height_observer.h>
#include <app/controllers/attitude_controller.h>
#include <app/controllers/rate_controller.h>

#include <data_model/data_ressources_registry.h>

class HeightController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    RateController          * _rate_controller;
    AttitudeController      * _attitude_controller;
    Marg                    * _marg;
    Barometer               * _baro;
    UltrasoundSensor        * _ultrasound;
    HeightObserver          * _observer;
    Pid                     * _speed_controller;
    Pid                     * _position_controller;
    Mutex                   * _mutex;
    float                     _period;

    float _height;
    float _height_speed;

    void run(void);

public:

    HeightController(float period, DataRessourcesRegistry * registry, Marg * marg, Barometer * baro, UltrasoundSensor * ultrasound,
                     AttitudeController * attitude_controller, RateController * rate_controller);

};