#pragma once

#include <stdint.h>
#include <string>

class Tick
{
private:

    uint64_t _ticks;

public:

    static Tick now();

    Tick(uint32_t seconds, uint32_t millis);
    Tick(uint64_t ticks);
    Tick(void);

    uint32_t seconds(void);
    uint32_t millis(void);
    uint64_t ticks(void);

    std::string str();

    Tick operator+(Tick);
    Tick operator-(Tick);
    bool operator>(Tick);
};