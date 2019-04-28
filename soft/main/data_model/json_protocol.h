#pragma once

#include <hal/udp_server.h>
#include <data_model/data_ressources_registry.h>

using namespace std;

class JsonDataProtocol
{

  private:

    UdpServer               * _udp;
    DataRessourcesRegistry  * _registry;

    void on_received_packet(string request, string& response);

  public:

    JsonDataProtocol(UdpServer * udp, DataRessourcesRegistry * registry);

};