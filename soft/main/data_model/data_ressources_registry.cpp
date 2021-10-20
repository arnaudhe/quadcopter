
#include <esp_spiffs.h>

#include <data_model/data_ressources_registry.h>
#include <data_model/ressource_enum.h>
#include <iostream>
#include <fstream>
#include <streambuf>

#include <hal/log.h>

DataRessourcesRegistry::DataRessourcesRegistry(string data_model_file) : 
    _mutex(),
    _callback(NULL)
{
    bool aborted = false;

    esp_vfs_spiffs_conf_t storage_conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };

    esp_err_t err = esp_vfs_spiffs_register(&storage_conf);

    if ( err != ESP_OK )
    {
        if ( err == ESP_FAIL )
        {
            LOG_ERROR("Failed to mount or format filesystem.");
        } else if ( err == ESP_ERR_NOT_FOUND ) {
            LOG_ERROR("Failed to find SPIFFS partition.");
        } else {
            LOG_ERROR("Failed to initialize SPIFFS (%s).", esp_err_to_name(err));
        }
        aborted = true;
    }

    if ( !aborted )
    {
        size_t total = 0, used = 0;
        err = esp_spiffs_info(NULL, &total, &used);
        if ( err != ESP_OK )
        {
            LOG_ERROR("Failed to get SPIFFS partition information (%s).", esp_err_to_name(err));
            aborted = true;
        } else {
            LOG_INFO("Partition size: total: %d, used: %d", total, used);
        }
    }

    FILE* file = NULL;

    if ( !aborted )
    {
        data_model_file = storage_conf.base_path + std::string("/") + data_model_file;
        file = fopen(data_model_file.c_str(), "r");
        if (file == NULL)
        {
            LOG_ERROR("Failed to open registry data model file.");
            aborted = true;
        }
    }

    if ( !aborted )
    {
        json data_model_json = json::parse(file);
        fclose(file);
        load_data_model(data_model_json);
        LOG_INFO("Data ressourecs registry initialized using data model file.");
    } else {
        LOG_ERROR("Data ressourecs registry initialization aborted.");
    }
    
    esp_vfs_spiffs_unregister(NULL);

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
                LOG_INFO("Load value %s", new_key.c_str());
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
                    _map[new_key] = new DataRessource(float(0.0f));
                    if (it.value().contains("min"))
                    {
                        _map[new_key]->set_minimum<float>(it.value().at("min").get<float>());
                    }
                    if (it.value().contains("max"))
                    {
                        _map[new_key]->set_maximum<float>(it.value().at("max").get<float>());
                    }
                }
                else if (it.value().at("type").get<string>() == "double")
                {
                    _map[new_key] = new DataRessource(double(0.0f));
                    if (it.value().contains("min"))
                    {
                        _map[new_key]->set_minimum<double>(it.value().at("min").get<double>());
                    }
                    if (it.value().contains("max"))
                    {
                        _map[new_key]->set_maximum<double>(it.value().at("max").get<double>());
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
                    LOG_ERROR("Invalid type of ressource %s", new_key.c_str());
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