#include "battery_monitor.hpp"
#include <iostream>

BatteryMonitor::BatteryMonitor(ADS1115 * ads, int channel) :
    Task("battery", false),
    _battery(ads, channel)
{
    return;
}

void BatteryMonitor::run(void)
{
    while (running())
    {
        Task::delay_ms(10 * 1000);
        _level =  _battery.get_level_percent();
        std::cout << "battery : " << _level << "%" << std::endl;
    }
}

int BatteryMonitor::level(void)
{
    return _level;
}
