#include <utils/byte_array.h>

#include <string.h>
#include <sstream>
#include <bitset>
#include <iostream>
#include <iomanip>

using namespace std;

ByteArray::ByteArray(const uint8_t * data, int length)
{
    append(data, length);
}

ByteArray::ByteArray(uint8_t b)
{
    _data.push_back(b);
}

ByteArray::ByteArray(void)
{
    _data.clear();
}

ByteArray::ByteArray(string str)
{
    append((uint8_t *)str.c_str(), str.length());
}

ByteArray ByteArray::from_hex(string hex)
{
    ByteArray ba;
    for (int i = 0; i < hex.length() / 2; i++)
    {
        ba.append(ByteArray::_char_to_int(hex[i * 2]) * 16 + ByteArray::_char_to_int(hex[i * 2 + 1]));
    }
    return ba;
}

uint8_t ByteArray::_char_to_int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<uint8_t>(c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<uint8_t>(c - 'A' + 10);
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<uint8_t>(c - 'a' + 10);
    }
    else
    {
        return 0;
    }
}

const uint8_t * ByteArray::data(void) const
{
    return _data.data();
}

int ByteArray::length() const
{
    return _data.size();
}

void ByteArray::clear(void)
{
    _data.clear();
}

void ByteArray::set_data(const uint8_t * data, int length)
{
    clear();
    append(data, length);
}

void ByteArray::set_data(uint8_t data)
{
    clear();
    append(data);
}

void ByteArray::set_data(string str)
{
    clear();
    append(str);
}

void ByteArray::append(uint8_t b)
{
    _data.push_back(b);
}

void ByteArray::append(ByteArray ba)
{
    append(ba.data(), ba.length());
}

void ByteArray::append(string str)
{
    append((const uint8_t *)str.c_str(), str.length());
}

void ByteArray::append(const uint8_t * data, int length)
{
    for (int i = 0; i < length; i++)
    {
        _data.push_back(data[i]);
    }
}

uint8_t ByteArray::at(int position) const
{
    if (position >= 0)
    {
        return _data.at(position);
    }
    else
    {
        return _data.at(_data.size() + position);
    }
}

ByteArray ByteArray::slice(int position, int length)
{
    int index;

    if (position >= 0)
    {
        index = position;
    }
    else
    {
        index = _data.size() + position;
    }

    return ByteArray(&_data.data()[index], length);
}

string ByteArray::hex(void)
{
    ostringstream ss;

    for(const uint8_t& b: _data)
    {
        ss << std::setfill('0') << std::setw(2) << std::hex << (int)b;
    }

    return ss.str();
}

uint8_t ByteArray::operator()(int position)
{
    return at(position);
}

ByteArray ByteArray::operator()(int position, int length)
{
    return slice(position, length);
}

ByteArray ByteArray::operator+(ByteArray ba)
{
    ByteArray ret(this->data(), this->length());
    ret.append(ba.data(), ba.length());
    return ret;
}

ByteArray ByteArray::operator+(uint8_t b)
{
    ByteArray ret(this->data(), this->length());
    ret.append(b);
    return ret;
}

bool ByteArray::operator==(ByteArray ba) const
{
    if (this->length() == ba.length())
    {
        if (memcmp(this->data(), ba.data(), this->length()) == 0)
        {
            return true;
        }
    }
    return false;
}

bool ByteArray::operator<(const ByteArray ba) const
{
    if (this->length() < ba.length())
    {
        return true;
    }
    else if (this->length() == ba.length())
    {
        for (int i = 0; i < this->length(); i++)
        {
            if (this->at(i) < ba.at(i))
            {
                return true;
            }
            else if (this->at(i) > ba.at(i))
            {
                return false;
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}