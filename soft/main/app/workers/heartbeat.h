#pragma once

#include <string>

#include <os/periodic_task.h>

#include <drv/SI4432.h>
#include <hal/udp_server.h>

#include <data_model/data_ressources_registry.h>

class Heartbeat : public PeriodicTask
{

  private:

    Si4432    * _si4432;
    UdpServer * _udp;

    void run();

  public:

    Heartbeat(float period, Si4432 * si4432, UdpServer * udp);
};