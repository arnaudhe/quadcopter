#pragma once

#include <data_model/data_ressources_registry.h>

using namespace std;

class JsonDataProtocol
{

  private:

    DataRessourcesRegistry  * _registry;

  public:

    JsonDataProtocol(DataRessourcesRegistry * registry);

    void on_received_packet(string request, string &response);

};