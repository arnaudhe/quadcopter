#pragma once

#include <map>
#include <iostream>
#include <cassert>
#include <functional>
#include <data_model/data_ressource.h>
#include <utils/json.hpp>
#include <os/mutex.h>
#include <utils/byte_array.h>

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
    map<ByteArray, string>      _key_lookup;
    Mutex                       _mutex;

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

    template <typename T>
    T * get_handle(string key);

    template <typename T>
    void internal_set_fast(T * handle, T value);

    void load_data_model(json * node, string current_key = "", ByteArray current_id = ByteArray());

    Status get_key_from_id(ByteArray id, string &key);

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

template <typename T>
inline T * DataRessourcesRegistry::get_handle(string key)
{
    T * handle;
    _mutex.lock();
    handle = _map[key]->get_handle<T>();
    _mutex.unlock();
    return handle;
}

template <typename T>
inline void DataRessourcesRegistry::internal_set_fast(T * handle, T value)
{
    _mutex.lock();
    *handle = value;
    _mutex.unlock();
}
