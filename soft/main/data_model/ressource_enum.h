#pragma once

#include <map>
#include <string>

using namespace std;

class EnumRessource
{

  private:

    map<string, int> _values;
    string           _value;

  public:

    EnumRessource(int, map<string, int>);
    EnumRessource(string, map<string, int>);

    bool set(string value);
    bool set(int value);

    bool check_value(string value);
    bool check_value(int value);

    template<typename T = string>
    T get();
};

template<>
inline string EnumRessource::get<string>()
{
    return _value;
}

template<>
inline int EnumRessource::get<int>()
{
    return _values[_value];
}