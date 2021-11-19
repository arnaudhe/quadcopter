#pragma once

#include <map>
#include <iostream>
#include <cassert>
#include <functional>
#include <data_model/data_ressource.h>
#include <hal/udp_server.h>
#include <utils/json.hpp>
#include <os/mutex.h>

using namespace std;
using json = nlohmann::json;

class DataRessourceRegistryStatus : public EnumRessource
{

  public:

    enum Status
    {
        SUCCESS         = 0,
        BAD_RESSOURCE   = 1,
        BAD_TYPE        = 2,
        BAD_PERMISSIONS = 3,
        BAD_VALUE       = 4
    };

  private:

    static map<string, int> create_values_map()
        {
          map<string, int> m;
          m["success"]          = DataRessourceRegistryStatus::SUCCESS;
          m["bad_ressource"]    = DataRessourceRegistryStatus::BAD_RESSOURCE;
          m["bad_type"]         = DataRessourceRegistryStatus::BAD_TYPE;
          m["bad_permissions"]  = DataRessourceRegistryStatus::BAD_PERMISSIONS;
          m["bad_value"]        = DataRessourceRegistryStatus::BAD_VALUE;
          return m;
        }

    const map<string, int> VALUES;

  public:

    DataRessourceRegistryStatus(int value) : EnumRessource(value, DataRessourceRegistryStatus::create_values_map()){}
    DataRessourceRegistryStatus(string value) : EnumRessource(value, DataRessourceRegistryStatus::create_values_map()){}
};

class DataRessourcesRegistry
{

  public:

    enum Status
    {
        SUCCESS         = 0,
        BAD_RESSOURCE   = 1,
        BAD_TYPE        = 2,
        BAD_VALUE       = 3,
        BAD_PERMISSIONS = 4
    };

  private:

    map<string, DataRessource*> _map;
    Mutex                       _mutex;
    function<void(string, DataRessource *)> _callback;

  public:

    DataRessourcesRegistry(string data_model_file);

    DataRessource::Type type(string key);

    template <typename T>
    DataRessourceRegistryStatus set(string key, T value);

    template <typename T>
    DataRessourceRegistryStatus get(string key, T &value);

    template <typename T>
    void internal_set(string key, T value);

    template <typename T>
    T internal_get(string key);

    void load_data_model(json * node, string current_key = "");

    void register_callback(function<void(string, DataRessource *)>);

};

template <typename T>
inline DataRessourceRegistryStatus DataRessourcesRegistry::set(string key, T value)
{
    if (_map.count(key) == 0)
    {
        return DataRessourceRegistryStatus(DataRessourceRegistryStatus::BAD_RESSOURCE);
    }

    _mutex.lock();
    DataRessourceRegistryStatus ret = DataRessourceRegistryStatus(_map[key]->set<T>(value).get());
    _mutex.unlock();

    return ret;
}

template <typename T>
inline DataRessourceRegistryStatus DataRessourcesRegistry::get(string key, T &value)
{
    if (_map.count(key) == 0)
    {
        return DataRessourceRegistryStatus(DataRessourceRegistryStatus::BAD_RESSOURCE);
    }

    _mutex.lock();
    DataRessourceRegistryStatus ret = DataRessourceRegistryStatus(_map[key]->get<T>(value).get());
    _mutex.unlock();

    return ret;
}

template <typename T>
inline void DataRessourcesRegistry::internal_set(string key, T value)
{
    _mutex.lock();
    _map[key]->internal_set<T>(value);
    _mutex.unlock();
}

template <typename T>
inline T DataRessourcesRegistry::internal_get(string key)
{
    T ret;
    _mutex.lock();
    ret = _map[key]->internal_get<T>();
    _mutex.unlock();
    return ret;
}