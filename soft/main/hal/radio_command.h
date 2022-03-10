#pragma once

#include <string>
#include <functional>

#include <os/task.h>

#include <data_model/data_ressources_registry.h>

#include <hal/radio.h>

using namespace std;

class RadioCommand : public Task
{
  private:

    int                      _socket;
    DataRessourcesRegistry * _registry;
    RadioBroker            * _radio;

    void run();

  public:

    RadioCommand(DataRessourcesRegistry * registry, RadioBroker * radio);
    ~RadioCommand();

};