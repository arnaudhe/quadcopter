#pragma once

#include <map>
#include <iostream>
#include <cassert>
#include <functional>

#include <data_model/ressource_enum.h>

using namespace std;

class DataRessourceStatus : public EnumRessource
{

  public:

    enum Status
    {
        SUCCESS         = 0,
        BAD_TYPE        = 2,
        BAD_PERMISSIONS = 3,
        BAD_VALUE       = 4
    };

  private:

    static map<string, int> create_values_map()
        {
          map<string, int> m;
          m["success"]          = DataRessourceStatus::SUCCESS;
          m["bad_type"]         = DataRessourceStatus::BAD_TYPE;
          m["bad_permissions"]  = DataRessourceStatus::BAD_PERMISSIONS;
          m["bad_value"]        = DataRessourceStatus::BAD_VALUE;
          return m;
        }

    const map<string, int> VALUES;

  public:

    DataRessourceStatus(int value) : EnumRessource(value, DataRessourceStatus::create_values_map()){}
    DataRessourceStatus(string value) : EnumRessource(value, DataRessourceStatus::create_values_map()){}
};

class DataRessource
{

  public:

    enum Type
    {
        INTEGER,
        FLOAT,
        DOUBLE,
        BOOL,
        ENUM
    };

  private:

    DataRessource::Type _type;
  
    int           _value_integer;
    float         _value_float;
    double        _value_double;
    bool          _value_bool;
    EnumRessource _value_enum;

    bool          _permission_read;
    bool          _permission_write;
    bool          _permission_notify;

    bool          _has_minimum;
    float         _minimum_float;
    double        _minimum_double;
    int           _minimum_int;
    bool          _has_maximum;
    float         _maximum_float;
    double        _maximum_double;
    int           _maximum_int;

    template <typename T>
    bool check_type();

    template <typename T>
    bool check_value(T value);

    DataRessource();

  public:

    DataRessource(int value);
    DataRessource(float value);
    DataRessource(double value);
    DataRessource(bool value);
    DataRessource(EnumRessource value);

    void set_permissions(bool read, bool write, bool notify);

    template <typename T>
    void set_minimum(T min_value);

    template <typename T>
    void set_maximum(T max_value);

    template <typename T>
    DataRessourceStatus set(T value);

    template <typename T>
    DataRessourceStatus get(T &value);

    template <typename T>
    void internal_set(T value);

    template <typename T>
    T internal_get();

    DataRessource::Type type();
};

template <>
inline void DataRessource::set_minimum<float>(float minimum)
{
    _has_minimum   = true;
    _minimum_float = minimum;
}

template <>
inline void DataRessource::set_minimum<double>(double minimum)
{
    _has_minimum    = true;
    _minimum_double = minimum;
}

template <>
inline void DataRessource::set_minimum<int>(int minimum)
{
    _has_minimum   = true;
    _minimum_int   = minimum;
}

template <>
inline void DataRessource::set_maximum<float>(float maximum)
{
    _has_maximum   = true;
    _maximum_float = maximum;
}

template <>
inline void DataRessource::set_maximum<double>(double maximum)
{
    _has_maximum    = true;
    _maximum_double = maximum;
}

template <>
inline void DataRessource::set_maximum<int>(int maximum)
{
    _has_maximum   = true;
    _maximum_int   = maximum;
}

template <>
inline bool DataRessource::check_type<int>()
{
    if ((_type == DataRessource::INTEGER) || (_type == DataRessource::ENUM))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <>
inline bool DataRessource::check_type<float>()
{
    if (_type == DataRessource::FLOAT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <>
inline bool DataRessource::check_type<double>()
{
    if (_type == DataRessource::DOUBLE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <>
inline bool DataRessource::check_type<bool>()
{
    if (_type == DataRessource::BOOL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <>
inline bool DataRessource::check_type<string>()
{
    if (_type == DataRessource::ENUM)
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <>
inline bool DataRessource::check_value<int>(int value)
{
    if (_type == DataRessource::INTEGER)
    {
        if (((_has_minimum) && (value < _minimum_int)) || ((_has_maximum) && (value > _maximum_int)))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else if (_type == DataRessource::ENUM)
    {
        return _value_enum.check_value(value);
    }
    else
    {
        return false;
    }
    
}

template <>
inline bool DataRessource::check_value<float>(float value)
{
    if (((_has_minimum) && (value < _minimum_float)) || ((_has_maximum) && (value > _maximum_float)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

template <>
inline bool DataRessource::check_value<double>(double value)
{
    if (((_has_minimum) && (value < _minimum_double)) || ((_has_maximum) && (value > _maximum_double)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

template <>
inline bool DataRessource::check_value<bool>(bool value)
{
    return true;
}

template <>
inline bool DataRessource::check_value<string>(string value)
{
    return _value_enum.check_value(value);
}

template <>
inline void DataRessource::internal_set<int>(int value)
{
    if (_type == DataRessource::INTEGER)
    {
        _value_integer = value;
    }
    else
    {
        _value_enum.set(value);
    }
}

template <>
inline void DataRessource::internal_set<float>(float value)
{
    _value_float = value;
}

template <>
inline void DataRessource::internal_set<double>(double value)
{
    _value_double = value;
}

template <>
inline void DataRessource::internal_set<bool>(bool value)
{
    _value_bool = value;
}

template <>
inline void DataRessource::internal_set<string>(string value)
{
    _value_enum.set(value);
}

template <>
inline int DataRessource::internal_get<int>()
{
    if (_type == DataRessource::INTEGER)
    {
        return _value_integer;
    }
    else
    {
        return _value_enum.get<int>();
    }
}

template <>
inline float DataRessource::internal_get<float>()
{
    return _value_float;
}

template <>
inline double DataRessource::internal_get<double>()
{
    return _value_double;
}

template <>
inline bool DataRessource::internal_get<bool>()
{
    return _value_bool;
}

template <>
inline string DataRessource::internal_get<string>()
{
    return _value_enum.get<string>();
}

template <typename T>
inline DataRessourceStatus DataRessource::set(T value)
{
    if (check_type<T>() == false)
    {
        return DataRessourceStatus(DataRessourceStatus::BAD_TYPE);
    }

    if (_permission_write == false)
    {
        return DataRessourceStatus(DataRessourceStatus::BAD_PERMISSIONS);
    }

    if (check_value<T>(value) == false)
    {
        return DataRessourceStatus(DataRessourceStatus::BAD_VALUE);
    }

    internal_set<T>(value);

    return DataRessourceStatus(DataRessourceStatus::SUCCESS);
}

template <typename T>
inline DataRessourceStatus DataRessource::get(T &value)
{
    if (check_type<T>() == false)
    {
        return DataRessourceStatus(DataRessourceStatus::BAD_TYPE);
    }

    if (_permission_read == false)
    {
        return DataRessourceStatus(DataRessourceStatus::BAD_PERMISSIONS);
    }

    value = internal_get<T>();

    return DataRessourceStatus(DataRessourceStatus::SUCCESS);
}