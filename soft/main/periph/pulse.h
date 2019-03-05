#pragma once

#include <driver/ledc.h>

using namespace std;

class Pulse
{
  
  private: 

    /* Attributes */

    ledc_timer_config_t   _timer_conf;       ///< timer
    ledc_channel_config_t _ledc_conf;
    uint16_t              _duty;
    int                   _gpio;
    int                   _channel;
    bool                  _init;

  public:

    /* Constructors */

    Pulse(uint16_t pulse_width_us, int gpio, int channel);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    esp_err_t deinit();

    esp_err_t set(uint16_t pulse_width_us);

};