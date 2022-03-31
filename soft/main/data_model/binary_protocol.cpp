#include <data_model/binary_protocol.h>
#include <hal/log.h>

BinaryDataProtocol::BinaryDataProtocol(DataRessourcesRegistry * registry)
{
    _registry = registry;
}

int BinaryDataProtocol::_get_value_length(Type type)
{
    int value_length = 0;

    switch (type)
    {
        case BinaryDataProtocol::EMPTY:
            value_length = 0;
            break;

        case BinaryDataProtocol::INTEGER:
            value_length = sizeof(int32_t);
            break;

        case BinaryDataProtocol::FLOAT:
            value_length = sizeof(float);
            break;

        case BinaryDataProtocol::ENUM:
            value_length = sizeof(uint8_t);
            break;

        case BinaryDataProtocol::BOOL:
            value_length = sizeof(uint8_t);
            break;

        case BinaryDataProtocol::STATUS:
            value_length = sizeof(uint8_t);
            break;

        case BinaryDataProtocol::DOUBLE:
            value_length = sizeof(double);
            break;
    
        default:
            break;
    }

    return value_length;
}

ByteArray BinaryDataProtocol::_read(ByteArray id)
{
    ByteArray ressource_response;
    string    key;

    ressource_response.append(id.length());
    ressource_response.append(id);

    /* Todo retrieve key from id */
    if (_registry->get_key_from_id(id, key) == DataRessourcesRegistry::Status::SUCCESS)
    {
        if (_registry->type(key) == DataRessource::INTEGER)
        {
            int32_t value = 0;
            DataRessourceRegistryStatus st = _registry->get<int>(key, value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            {
                ressource_response.append(BinaryDataProtocol::INTEGER);
                ressource_response.append((uint8_t *)&value, sizeof(value));
            }
            else
            {
                ressource_response.append(BinaryDataProtocol::STATUS);
                ressource_response.append((uint8_t)st.get<int>());
            }
        }
        else if (_registry->type(key) == DataRessource::FLOAT)
        {
            float value = 0.0f;
            DataRessourceRegistryStatus st = _registry->get<float>(key, value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            {
                ressource_response.append(BinaryDataProtocol::FLOAT);
                ressource_response.append((uint8_t *)&value, sizeof(value));
            }
            else
            {
                ressource_response.append(BinaryDataProtocol::STATUS);
                ressource_response.append((uint8_t)st.get<int>());
            }
        }
        else if (_registry->type(key) == DataRessource::DOUBLE)
        {
            double value = 0.0;
            DataRessourceRegistryStatus st = _registry->get<double>(key, value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            {
                ressource_response.append(BinaryDataProtocol::DOUBLE);
                ressource_response.append((uint8_t *)&value, sizeof(value));
            }
            else
            {
                ressource_response.append(BinaryDataProtocol::STATUS);
                ressource_response.append((uint8_t)st.get<int>());
            }
        }
        else if (_registry->type(key) == DataRessource::BOOL)
        {
            bool value = false;
            DataRessourceRegistryStatus st = _registry->get<bool>(key, value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            {
                ressource_response.append(BinaryDataProtocol::BOOL);
                ressource_response.append((uint8_t)value);
            }
            else
            {
                ressource_response.append(BinaryDataProtocol::STATUS);
                ressource_response.append((uint8_t)st.get<int>());
            }
        }
        else if (_registry->type(key) == DataRessource::ENUM)
        {
            int value = 0;
            DataRessourceRegistryStatus st = _registry->get<int>(key, value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            {
                ressource_response.append(BinaryDataProtocol::ENUM);
                ressource_response.append((uint8_t)value);
            }
            else
            {
                ressource_response.append(BinaryDataProtocol::STATUS);
                ressource_response.append((uint8_t)st.get<int>());
            }
        }
        else
        {
            LOG_ERROR("Invalid type of ressource with id %s", id.hex().c_str());
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)DataRessourceRegistryStatus::BAD_RESSOURCE);
        }
    }
    else
    {
        LOG_ERROR("Unknown ressource with id %s", id.hex().c_str());
        ressource_response.append(BinaryDataProtocol::STATUS);
        ressource_response.append((uint8_t)DataRessourceRegistryStatus::BAD_RESSOURCE);
    }

    return ressource_response;
}

ByteArray BinaryDataProtocol::_write(ByteArray id, BinaryDataProtocol::Type type, ByteArray value_bytes)
{
    ByteArray ressource_response;
    string    key;

    ressource_response.append(id.length());
    ressource_response.append(id);

    /* Todo retrieve key from id */
    if (_registry->get_key_from_id(id, key) == DataRessourcesRegistry::Status::SUCCESS)
    {
        if (_registry->type(key) == DataRessource::INTEGER)
        {
            int32_t * value = (int32_t *)(value_bytes.data());
            DataRessourceRegistryStatus st = _registry->set<int>(key, (int)(*value));
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)st.get<int>());
        }
        else if (_registry->type(key) == DataRessource::FLOAT)
        {
            float * value = (float *)(value_bytes.data());
            DataRessourceRegistryStatus st = _registry->set<float>(key, *value);
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)st.get<int>());
        }
        else if (_registry->type(key) == DataRessource::DOUBLE)
        {
            double * value = (double *)(value_bytes.data());
            DataRessourceRegistryStatus st = _registry->set<double>(key, *value);
            if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)st.get<int>());
        }
        else if (_registry->type(key) == DataRessource::BOOL)
        {
            const uint8_t * value = value_bytes.data();
            DataRessourceRegistryStatus st = _registry->set<bool>(key, (int)(*value));
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)st.get<int>());

        }
        else if (_registry->type(key) == DataRessource::ENUM)
        {
            const uint8_t * value = value_bytes.data();
            DataRessourceRegistryStatus st = _registry->set<int>(key, (int)(*value));
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)st.get<int>());
        }
        else
        {
            LOG_ERROR("Invalid type of ressource for key %s", id.hex());
            ressource_response.append(BinaryDataProtocol::STATUS);
            ressource_response.append((uint8_t)DataRessourceRegistryStatus::BAD_RESSOURCE);
        }
    }
    else
    {
        LOG_ERROR("Unknown ressource with id %s", id.hex().c_str());
        ressource_response.append(BinaryDataProtocol::STATUS);
        ressource_response.append((uint8_t)DataRessourceRegistryStatus::BAD_RESSOURCE);
    }

    return ressource_response;
}

void BinaryDataProtocol::on_received_packet(ByteArray request, ByteArray &response)
{
    int offset;
    BinaryDataProtocol::Header response_header;

    response = ByteArray();

    BinaryDataProtocol::Header * request_header = (BinaryDataProtocol::Header *)(request.data());

    BinaryDataProtocol::Direction direction = (BinaryDataProtocol::Direction)(request_header->direction);
    BinaryDataProtocol::Command   command   = (BinaryDataProtocol::Command)(request_header->command);

    if (direction == BinaryDataProtocol::REQUEST)
    {
        response_header.command   = command;
        response_header.direction = Direction::RESPONSE;
        response_header.count     = request_header->count;
        response_header.sequence  = request_header->sequence;

        response = ByteArray((uint8_t *)&response_header, sizeof(response_header));

        offset = sizeof(BinaryDataProtocol::Header);

        for (int i = 0; i < request_header->count; i++)
        {
            uint8_t id_len = request(offset);
            offset += 1;

            ByteArray id = request(offset, id_len);
            offset += id_len;

            BinaryDataProtocol::Type type = (BinaryDataProtocol::Type)(request(offset));
            offset += 1;

            int value_length = _get_value_length(type);
            ByteArray value = request(offset, value_length);
            offset += value_length;

            if (command == BinaryDataProtocol::READ)
            {
                response = response + _read(id);
            }
            else if (command == BinaryDataProtocol::WRITE)
            {
                response = response + _write(id, type, value);
            }
            else
            {
                LOG_ERROR("Invalid command %d", command);
            }
        }
    }
    else
    {
        response = ByteArray();
    }

    LOG_DEBUG("response %s", response.hex().c_str());
}
