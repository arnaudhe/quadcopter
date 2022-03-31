#pragma once

#include <vector>
#include <string>
#include <stdint.h>

using namespace std;

class ByteArray
{

private:

    vector<uint8_t> _data;

    static uint8_t _char_to_int(char c);

public:

    ByteArray(const uint8_t * data, int length);
    ByteArray(uint8_t);
    ByteArray(string str);
    ByteArray();

    /* Methods to get pointer to data and length of data */
    const uint8_t * data(void) const;
    int length() const;

    /* Hex string representation of bytes */
    static ByteArray from_hex(string hex);
    string hex();

    /* Append data methods and operators */
    void append(uint8_t b);
    void append(const uint8_t * data, int length);
    void append(ByteArray ba);
    ByteArray operator+(ByteArray);
    ByteArray operator+(uint8_t);

    /* Get element method and operator */
    uint8_t at(int position) const;
    uint8_t operator()(int position);

    /* Get slice (sub-array) method and operator */
    ByteArray slice(int position, int length);
    ByteArray operator()(int position, int length);

    /* Comparison operators */
    bool operator==(ByteArray ba) const;
    bool operator<(const ByteArray ba) const;

};
