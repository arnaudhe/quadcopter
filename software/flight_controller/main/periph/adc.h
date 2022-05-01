#pragma once

#include <driver/adc.h>
#include "esp_adc_cal.h"

using namespace std;

class Adc
{

  public:

    enum Unit
    {
        UNIT_1 = 1,
        UNIT_2
    };

    enum Channel
    {
        CHANNEL_0 = 0,
        CHANNEL_1,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4,
        CHANNEL_5,
        CHANNEL_6,
        CHANNEL_7
    };

    Adc(Adc::Unit unit);

    void configure_channel(Adc::Channel channel);

    float sample(Adc::Channel channel);

  private:

    Adc::Unit                       _unit;
    esp_adc_cal_characteristics_t   _characteristics;
};
