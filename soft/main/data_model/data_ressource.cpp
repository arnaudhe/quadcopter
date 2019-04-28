#include <data_model/data_ressource.h>

DataRessource::DataRessource():
    _value_enum(0, map<string, int>())
{
    _permission_read   = true;
    _permission_write  = true;
    _permission_notify = false;

    _has_minimum       = false;
    _has_maximum       = false;
}

DataRessource::DataRessource(int value):
    DataRessource()
{
    _value_integer = value;
    _type = DataRessource::INTEGER;
}

DataRessource::DataRessource(float value):
    DataRessource()
{
    _value_integer = value;
    _type = DataRessource::FLOAT;
}

DataRessource::DataRessource(bool value):
    DataRessource()
{
    _value_bool = value;
    _type = DataRessource::BOOL;
}

DataRessource::DataRessource(EnumRessource value):
    DataRessource()
{
    _value_enum = value;
    _type = DataRessource::ENUM;
}

void DataRessource::set_permissions(bool read, bool write, bool notify)
{
    _permission_read   = read;
    _permission_write  = write;
    _permission_notify = notify;
}

DataRessource::Type DataRessource::type()
{
    return _type;
}