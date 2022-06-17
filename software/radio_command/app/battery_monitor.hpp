#pragma once

#include <os/task.hpp>
#include <drv/ADS1115.hpp>
#include <hal/battery.hpp>

class BatteryMonitor : public Task
{

private:

    Battery _battery;

    void run(void);

public:

    BatteryMonitor(ADS1115 * ads, int channel);

};