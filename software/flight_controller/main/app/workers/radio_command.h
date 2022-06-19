#pragma once

#include <string>
#include <functional>

#include <os/task.h>

#include <data_model/data_ressources_registry.h>

#include <app/workers/broker.h>

using namespace std;

class RadioCommand : public Task
{
  private:

    struct __attribute__((packed)) CommandsPayload
    {
        uint8_t armed;
        float   roll;
        float   pitch;
        float   yaw;
        float   throttle;
    };

    struct __attribute__((packed)) StatusPayload
    {
        uint8_t armed;
        uint8_t battery;
        uint8_t link_quality;
        uint8_t recording;
        uint8_t camera_connected;
        uint8_t camera_battery;
    };

    DataRessourcesRegistry * _registry;
    Broker                 * _broker;

    void run();

  public:

    RadioCommand(DataRessourcesRegistry * registry, Broker * broker);

};