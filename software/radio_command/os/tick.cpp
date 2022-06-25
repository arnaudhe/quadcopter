#include "tick.hpp"

#include <sys/time.h>
#include <stdlib.h>

Tick Tick::now(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return Tick(tv.tv_sec, tv.tv_usec / 1000);
}

Tick::Tick(uint32_t seconds, uint32_t millis)
{
    _ticks = seconds * 1000 + millis;
}

Tick::Tick(uint64_t ticks)
{
    _ticks = ticks;
}

Tick::Tick(void)
{
    _ticks = Tick::now().ticks();
}

uint32_t Tick::seconds(void)
{
    return _ticks / 1000;
}

uint32_t Tick::millis(void)
{
    return _ticks % 1000;
}

uint64_t Tick::ticks(void)
{
    return _ticks;
}

std::string Tick::str()
{
    return std::to_string(seconds()) + "." + std::to_string(millis());
}

Tick Tick::operator+(Tick add)
{
    return Tick(_ticks + add.ticks());
}

Tick Tick::operator-(Tick sub)
{
    if (_ticks > sub.ticks())
    {
        return Tick(_ticks - sub.ticks());
    }
    else
    {
        return Tick(0);
    }
}

bool Tick::operator>(Tick other)
{
    return _ticks > other.ticks();
}
