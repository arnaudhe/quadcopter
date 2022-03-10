#include <hal/udp_server.h>
#include <data_model/data_ressources_registry.h>
#include <data_model/json_protocol.h>

int main(int argc, char ** argv)
{
    UdpServer * server = new UdpServer("control_server", 5001);
    DataRessourcesRegistry * registry = new DataRessourcesRegistry("../../../../../data_model/data_model.json");
    JsonDataProtocol * protocol = new JsonDataProtocol(server, registry);

    server->start();

    return 0;
}