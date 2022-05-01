#include <data_model/ressource_enum.h>

EnumRessource::EnumRessource(int value, map<string, int> values)
{
    _values = values;
    set(value);
}

EnumRessource::EnumRessource(string value, map<string, int> values)
{
    _values = values;
    set(value);
}

bool EnumRessource::check_value(string value)
{
    if (_values.count(value))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EnumRessource::check_value(int value)
{
    for (auto &i : _values) 
    {
        if (i.second == value) 
        {
            return true;
        }
    }
    return false;
}

bool EnumRessource::set(string value)
{
    if (_values.count(value))
    {
        _value = value;
        return true;
    }
    else
    {
        return false;
    }
}

bool EnumRessource::set(int value)
{
    for (auto &i : _values) 
    {
        if (i.second == value) 
        {
            _value = i.first;
            return true;
        }
    }
    return false;
}
