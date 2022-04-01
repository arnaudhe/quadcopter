#include <data_model/data_ressources_registry.h>
#include <data_model/binary_protocol.h>

#include <iostream>
#include <data_model/binary_protocol.h>

int main(int argc, char ** argv)
{
    DataRessourcesRegistry * registry = new DataRessourcesRegistry("data_model.json");
    BinaryDataProtocol     * protocol = new BinaryDataProtocol(registry);
    ByteArray                response;

    protocol->on_received_packet(ByteArray::from_hex("22ab0201020002020100"), response);

    std::cout << response.hex() << std::endl;

    return 0;
}
