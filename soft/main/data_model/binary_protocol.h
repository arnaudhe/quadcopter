#pragma once

#include <data_model/data_ressources_registry.h>
#include <utils/byte_array.h>

class BinaryDataProtocol
{
  private:

    enum Command
    {
      READ   = 1,
      WRITE  = 2,
      NOTIFY = 3,
    };

    enum Direction
    {
      REQUEST  = 0,
      RESPONSE = 1,
    };

    enum Type
    {
      EMPTY   = 0,
      INTEGER = 1,
      FLOAT   = 2,
      ENUM    = 3,
      BOOL    = 4,
      STATUS  = 5,
      DOUBLE  = 6,
    };

    struct Header
    {
      uint8_t count     : 4;
      uint8_t direction : 1;
      uint8_t command   : 3;
      uint8_t sequence;
    };

    DataRessourcesRegistry  * _registry;

    int _get_value_length(Type type);

    ByteArray _read(ByteArray id);

    ByteArray _write(ByteArray id, Type type, ByteArray value);

  public:

    BinaryDataProtocol(DataRessourcesRegistry * registry);

    void on_received_packet(ByteArray request, ByteArray &response);

};
