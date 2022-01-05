#pragma once

#include <data_model/data_ressources_registry.h>
#include <hal/udp_server.h>
#include <os/task.h>

class Telemetry : public Task
{

private:

    DataRessourcesRegistry * _registry;
    UdpServer              * _udp;

    void run(void);

public:

    Telemetry(DataRessourcesRegistry * registy, int period, UdpServer * udp);

};