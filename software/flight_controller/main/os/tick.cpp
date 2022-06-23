#include "tick.h"

#include <sys/time.h>
#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

Tick IRAM_ATTR Tick::now(void)
{
    return Tick(xTaskGetTickCount());
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

uint32_t IRAM_ATTR Tick::seconds(void)
{
    return _ticks / 1000;
}

uint32_t IRAM_ATTR Tick::millis(void)
{
    return _ticks % 1000;
}

uint64_t IRAM_ATTR Tick::ticks(void)
{
    return _ticks;
}

std::string Tick::str()
{
    return std::to_string(seconds()) + "." + std::to_string(millis());
}

Tick IRAM_ATTR Tick::operator+(Tick add)
{
    return Tick(_ticks + add.ticks());
}

Tick IRAM_ATTR Tick::operator-(Tick sub)
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

bool IRAM_ATTR Tick::operator>(Tick other)
{
    return _ticks > other.ticks();
}
