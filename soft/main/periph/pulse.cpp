#include <periph/pulse.h>

#define PULSE_BIT_SIZE             15
#define PULSE_MIN_VALUE_us         1000
#define PULSE_MAX_VALUE_us         2000
#define PULSE_FREQUENCY_Hz         50
#define PULSE_PERIOD_us            20000

#define PULSE_DUTY(a)                 ((((1 << PULSE_BIT_SIZE) - 1) * a) / PULSE_PERIOD_us)

pulse::pulse(uint16_t pulse_width_us, int gpio)
{
    _duty = PULSE_DUTY(pulse_width_us);
    _gpio = gpio;
    _init = false;
}

esp_err_t pulse::init()
{
    if (!_init)
    {
        _timer_conf.bit_num    = LEDC_TIMER_15_BIT;
        _timer_conf.freq_hz    = PULSE_FREQUENCY_Hz;
        _timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
        _timer_conf.timer_num  = LEDC_TIMER_0;
        ledc_timer_config(&_timer_conf);

        _ledc_conf.channel    = LEDC_CHANNEL_0;
        _ledc_conf.duty       = _duty;
        _ledc_conf.gpio_num   = _gpio;
        _ledc_conf.intr_type  = LEDC_INTR_DISABLE;
        _ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
        _ledc_conf.timer_sel  = LEDC_TIMER_0;
        ledc_channel_config(&_ledc_conf);

        _init = true;
    }

    return ESP_OK;
}

esp_err_t pulse::deinit()
{
    return ESP_OK;
}

esp_err_t pulse::set(uint16_t pulse_width_us)
{
    if (_init)
    {
        _duty = PULSE_DUTY(pulse_width_us);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, _duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    }

    return ESP_OK;
}



