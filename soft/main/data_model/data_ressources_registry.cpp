#include <data_model/data_ressources_registry.h>
#include <data_model/ressource_enum.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <data_model/data_model_json.h>

#include <esp_log.h>

DataRessourcesRegistry::DataRessourcesRegistry(string data_model_file) : 
    _callback(NULL)
{
    // ifstream t(data_model_file);
    // string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

    json data_model_json = json::parse(JSON_DATA_MODEL);

    load_data_model(data_model_json);
}

void DataRessourcesRegistry::load_data_model(json node, string current_key)
{
    for (json::iterator it = node.begin(); it != node.end(); ++it) 
    {
        string new_key = current_key + it.key();
        if (it.value().is_object())
        {
            if (it.value().contains("type"))
            {
                ESP_LOGI("Registry", "Load value %s", new_key.c_str());
                if (it.value().at("type").get<string>() == "integer")
                {
                    _map[new_key] = new DataRessource(0);
                    if (it.value().contains("min"))
                    {
                        _map[new_key]->set_minimum<int>(it.value().at("min").get<int>());
                    }
                    if (it.value().contains("max"))
                    {
                        _map[new_key]->set_maximum<int>(it.value().at("max").get<int>());
                    }
                }
                else if (it.value().at("type").get<string>() == "float")
                {
                    _map[new_key] = new DataRessource(0.0f);
                    if (it.value().contains("min"))
                    {
                        _map[new_key]->set_minimum<float>(it.value().at("min").get<float>());
                    }
                    if (it.value().contains("max"))
                    {
                        _map[new_key]->set_maximum<float>(it.value().at("max").get<float>());
                    }
                }
                else if (it.value().at("type").get<string>() == "bool")
                {
                    _map[new_key] = new DataRessource(false);
                }
                else if (it.value().at("type").get<string>() == "enum")
                {
                    EnumRessource e(0, it.value().at("values").get<map<string, int>>());
                    _map[new_key] = new DataRessource(e);
                }
                else
                {
                    ESP_LOGE("Registry", "Invalid type of ressource %s", new_key.c_str());
                    break;
                }
                bool permission_read   = false;
                bool permission_write  = false;
                bool permission_notify = false;

                if (it.value().contains("permissions"))
                {
                    if (it.value().at("permissions").is_array())
                    {
                        for (json::iterator it_perm = it.value().at("permissions").begin(); it_perm != it.value().at("permissions").end(); ++it_perm) 
                        {
                            string permission = *it_perm;
                            if (permission == "read")
                            {
                                permission_read = true;
                            }
                            else if (permission == "write")
                            {
                                permission_write = true;
                            }
                            else if (permission == "notify")
                            {
                                permission_notify = true;
                            }
                        }
                        _map[new_key]->set_permissions(permission_read, permission_write, permission_notify);
                    }
                }
            }
            else
            {
                load_data_model(it.value(), new_key + '.');
            }
        }
    }
}

DataRessource::Type DataRessourcesRegistry::type(string key)
{
    if (_map.count(key) == 1)
    {
        return _map[key]->type();
    }
    else 
    {
        return DataRessource::INTEGER;
    }
}

void DataRessourcesRegistry::register_callback(function<void(string, DataRessource *)> callback)
{
    _callback = callback;
}