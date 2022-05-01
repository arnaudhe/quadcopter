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

    DataRessourcesRegistry * _registry;
    Broker                 * _broker;

    void run();

  public:

    RadioCommand(DataRessourcesRegistry * registry, Broker * broker);

};