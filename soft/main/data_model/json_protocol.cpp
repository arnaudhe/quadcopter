#include <data_model/json_protocol.h>
#include <utils/json.hpp>
#include <iostream>
#include <hal/log.h>

using json = nlohmann::json;

JsonDataProtocol::JsonDataProtocol(DataRessourcesRegistry * registry)
{
    _registry = registry;
}

void JsonDataProtocol::on_received_packet(string request, string &response)
{
    json json_response, json_status;
    auto json_root = json::parse(request);

    if (!json_root.is_null() && json_root.type() == json::value_t::object)
    {
        string command    = json_root["command"].get<string>();
        int    sequence   = json_root["sequence"].get<int>();
        string direction  = json_root["direction"].get<string>();
        json   ressources = json_root["ressources"];

        if (command == "read")
        {
            json ressources_values;
            for (json::iterator it = ressources.begin(); it != ressources.end(); ++it) 
            {
                string key = *it;
                if (_registry->type(key) == DataRessource::INTEGER)
                {
                    int value = 0;
                    DataRessourceRegistryStatus st = _registry->get<int>(key, value);
                    json_status[key] = st.get();
                    if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
                    {
                        ressources_values[key] = value;
                    }
                }
                else if (_registry->type(key) == DataRessource::FLOAT)
                {
                    float value = 0.0;
                    DataRessourceRegistryStatus st = _registry->get<float>(key, value);
                    json_status[key] = st.get();
                    if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
                    {
                        ressources_values[key] = value;
                    }
                }
                else if (_registry->type(key) == DataRessource::DOUBLE)
                {
                    double value = 0.0;
                    DataRessourceRegistryStatus st = _registry->get<double>(key, value);
                    json_status[key] = st.get();
                    if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
                    {
                        ressources_values[key] = value;
                    }
                }
                else if (_registry->type(key) == DataRessource::BOOL)
                {
                    bool value = false;
                    DataRessourceRegistryStatus st = _registry->get<bool>(key, value);
                    json_status[key] = st.get();
                    if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
                    {
                        ressources_values[key] = value;
                    }
                }
                else if ((_registry->type(key) == DataRessource::ENUM) || (_registry->type(key) == DataRessource::STRING))
                {
                    string value = "";
                    DataRessourceRegistryStatus st = _registry->get<string>(key, value);
                    json_status[key] = st.get();
                    if (st.get<int>() == DataRessourceRegistryStatus::SUCCESS)
                    {
                        ressources_values[key] = value;
                    }
                }
                else
                {
                    LOG_ERROR("Invalid type of ressource for key \"%s\"", key.c_str());
                }
            }
            json_response["command"]    = "read";
            json_response["sequence"]   = sequence;
            json_response["direction"]  = "response";
            json_response["status"]     = json_status;
            json_response["ressources"] = ressources_values;

            response = json_response.dump();
        }
        else if (command == "write")
        {
            for (json::iterator it = ressources.begin(); it != ressources.end(); ++it) 
            {
                if (it.value().type() == json::value_t::number_float)
                {
                    if ( _registry->type(it.key()) ==  DataRessource::FLOAT )
                    {
                        json_status[it.key()] = _registry->set(it.key(), it.value().get<float>()).get();
                    } else if ( _registry->type(it.key()) ==  DataRessource::DOUBLE ) {
                        json_status[it.key()] = _registry->set(it.key(), it.value().get<double>()).get();
                    } else {
                        LOG_ERROR("Write error, data type mismatch for key:%s", it.key().c_str());
                    }
                }
                else if ((it.value().type() == json::value_t::number_integer) ||
                         (it.value().type() == json::value_t::number_unsigned))
                {
                    json_status[it.key()] = _registry->set(it.key(), it.value().get<int>()).get();
                }
                else if (it.value().type() == json::value_t::boolean)
                {
                    json_status[it.key()] = _registry->set(it.key(), it.value().get<bool>()).get();
                }
                else if (it.value().type() == json::value_t::string)
                {
                    json_status[it.key()] = _registry->set(it.key(), it.value().get<string>()).get();
                }
                else
                {
                    json_status[it.key()] = DataRessourceRegistryStatus(DataRessourceRegistryStatus::BAD_TYPE).get();
                }
            }
            json_response["command"]   = "write";
            json_response["sequence"]  = sequence;
            json_response["direction"] = "response";
            json_response["status"]    = json_status;

            response = json_response.dump();
        }
    }
}