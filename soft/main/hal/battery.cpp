#include <hal/battery.h>
#include <hal/log.h>

#define BATTERY_CELL_HIGH_VOLTAGE       (4.2f)
#define BATTERY_CELL_LOW_VOLTAGE        (3.4f)

Battery::Battery(Adc * adc, Adc::Channel channel, int cells, float measure_divider)
{
    _adc             = adc;
    _channel         = channel;
    _cells           = cells;
    _measure_divider = measure_divider;

    _adc->configure_channel(_channel);
}

float Battery::get_level(void)
{
    float adc_voltage = _adc->sample(_channel);
    float measure = adc_voltage * _measure_divider;
    float measure_per_cell = measure / (float)_cells;
    float percent;

    LOG_DEBUG("Battery voltage %.3f V", measure);

    percent = (100.0 * (measure_per_cell - BATTERY_CELL_LOW_VOLTAGE)) / (BATTERY_CELL_HIGH_VOLTAGE - BATTERY_CELL_LOW_VOLTAGE);

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