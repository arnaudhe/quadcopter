#pragma once

#include <string>
#include <functional>

#include <os/task.h>

#include <data_model/data_ressources_registry.h>

using namespace std;

class RadioCommand : public Task
{
  private:

    int                      _socket;
    DataRessourcesRegistry * _registry;

    void run();

  public:

    RadioCommand(DataRessourcesRegistry * registry);
    ~RadioCommand();

};