#pragma once

#include <string>

#include <os/periodic_task.h>

#include <hal/radio.h>
#include <hal/udp_server.h>
#include <hal/motor.h>

#include <data_model/data_ressources_registry.h>

class Heartbeat : public PeriodicTask
{

  private:

    RadioBroker  * _radio;
    UdpServer    * _udp;
    Motor        * _motor;

    void run();

  public:

    Heartbeat(float period, RadioBroker * radio, UdpServer * udp, Motor * _motor);
};