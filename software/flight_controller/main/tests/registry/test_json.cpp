#include <data_model/data_ressources_registry.h>
#include <data_model/json_protocol.h>

int main(int argc, char ** argv)
{
    DataRessourcesRegistry * registry = new DataRessourcesRegistry("data_model.json");
    JsonDataProtocol       * protocol = new JsonDataProtocol(registry);
    string                   response;

    protocol->on_received_packet("{ \"command\" : \"write\","                   \
                                 "      \"direction\" : \"request\","           \
                                 "      \"sequence_number\" : 14,"              \
                                 "      \"ressources\" : {"                     \
                                 "           \"control.mode\" : \"attitude\""   \
                                 "      }"                                      \
                                 "}", response);

    return 0;
}
