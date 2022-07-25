#include "battery.hpp"

#include <iostream>

#define POWER_SUPPLY_VOLTAGE            (4.096f)
#define BATTERY_CELL_HIGH_VOLTAGE       (4.2f)
#define BATTERY_CELL_LOW_VOLTAGE        (3.4f)
#define PLATFORM_BATTERY_DIVIDER        (1.3f)
#define PLATFORM_ADC_FULL_SCALE         32767

Battery::Battery(ADS1115 * ads, int channel)
{
    _ads     = ads;
    _channel = channel;
}

float Battery::get_voltage(void)
{
    int sample = _ads->sample_channel(_channel);

    return POWER_SUPPLY_VOLTAGE * (float)sample * PLATFORM_BATTERY_DIVIDER / PLATFORM_ADC_FULL_SCALE;
}

float Battery::get_level_percent(void)
{
    float voltage = get_voltage();

    float percent = (100.0 * (voltage - BATTERY_CELL_LOW_VOLTAGE)) / (BATTERY_CELL_HIGH_VOLTAGE - BATTERY_CELL_LOW_VOLTAGE);

    if (percent > 100.0)
    {
        percent = 100.0;
    }
    else if (percent < 0.0)
    {
        percent = 0.0f;
    }

    return percent;
}
