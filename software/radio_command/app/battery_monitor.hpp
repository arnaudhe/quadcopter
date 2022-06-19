#pragma once

#include <os/task.hpp>
#include <drv/ADS1115.hpp>
#include <hal/battery.hpp>

class BatteryMonitor : public Task
{

private:

    Battery _battery;
    int     _level;

    void run(void);

public:

    BatteryMonitor(ADS1115 * ads, int channel);

    int level(void);

};