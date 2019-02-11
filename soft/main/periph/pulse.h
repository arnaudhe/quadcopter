#pragma once

#include <driver/ledc.h>

using namespace std;

class pulse
{
  
  private: 

    /* Attributes */

    ledc_timer_config_t   _timer_conf;       ///< timer
    ledc_channel_config_t _ledc_conf;
    uint16_t              _duty;
    int                   _gpio;
    bool                  _init;

  public:

    /* Constructors */

    pulse(uint16_t pulse_width_us, int gpio);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    esp_err_t deinit();

    esp_err_t set(uint16_t pulse_width_us);

};