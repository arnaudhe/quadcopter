#pragma once

#include <string>

#include <os/periodic_task.h>

#include <hal/motor.h>
#include <app/workers/broker.h>

#include <data_model/data_ressources_registry.h>

class Heartbeat : public PeriodicTask
{

  private:

    Motor  * _motor;
    Broker * _broker;

    void run();

  public:

    Heartbeat(float period, Broker * broker, Motor * _motor);
};