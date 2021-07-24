#pragma once

#include <hal/marg.h>
#include <hal/barometer.h>
#include <hal/ultrasound.h>
#include <hal/motor.h>

#include <os/periodic_task.h>

#include <data_model/data_ressources_registry.h>

class DataRecorder : public PeriodicTask
{

private:

    Marg  * _marg;
    Motor * _front_left;
    Motor * _front_right;
    Motor * _rear_left;
    Motor * _rear_right;

    void run(void);

public:

    DataRecorder(Motor * front_left, Motor * front_right, Motor * rear_left, Motor * rear_right, Marg * marg);

};