#include <periph/adc.h>

#include <hal/log.h>

#include <platform.h>

Adc::Adc(Adc::Unit unit)
{
    _unit = unit;

    if (unit == Adc::UNIT_1)
    {
        adc1_config_width(ADC_WIDTH_12Bit);
    }
}

void Adc::configure_channel(Adc::Channel channel)
{
    if (_unit == Adc::UNIT_1)
    {
        adc1_config_channel_atten((adc1_channel_t)channel, ADC_ATTEN_11db);
    }
    else
    {
        adc2_config_channel_atten((adc2_channel_t)channel, ADC_ATTEN_11db );
    }
}

float Adc::sample(Adc::Channel channel)
{
    int sample;

    if (_unit == Adc::UNIT_1)
    {
        sample = adc1_get_raw((adc1_channel_t)channel);
    }
    else
    {
        adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_12Bit, &sample);
    }

    esp_adc_cal_characterize((adc_unit_t)_unit, ADC_ATTEN_11db, ADC_WIDTH_12Bit, PLATFORM_ADC_VREF, &_characteristics);

    return (float)esp_adc_cal_raw_to_voltage(sample, &_characteristics) / 1000;
}
