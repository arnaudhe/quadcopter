#include <periph/pulse.h>

#define PULSE_TIMER                LEDC_TIMER_0
#define PULSE_BIT_SIZE             15
#define PULSE_FREQUENCY_Hz         50
#define PULSE_PERIOD_us            20000

#define PULSE_DUTY(a)              ((((1 << PULSE_BIT_SIZE) - 1) * a) / PULSE_PERIOD_us)

Pulse::Pulse(uint16_t pulse_width_us, int gpio, int channel)
{
    _duty    = PULSE_DUTY(pulse_width_us);
    _gpio    = gpio;
    _channel = channel;
    _init    = false;
}

esp_err_t Pulse::init()
{
    if (!_init)
    {
        _timer_conf.clk_cfg         = LEDC_AUTO_CLK;
        _timer_conf.duty_resolution = (ledc_timer_bit_t)PULSE_BIT_SIZE;
        _timer_conf.freq_hz         = PULSE_FREQUENCY_Hz;
        _timer_conf.speed_mode      = LEDC_HIGH_SPEED_MODE;
        _timer_conf.timer_num       = PULSE_TIMER;
        ledc_timer_config(&_timer_conf);

        _ledc_conf.channel    = (ledc_channel_t)_channel;
        _ledc_conf.duty       = _duty;
        _ledc_conf.gpio_num   = _gpio;
        _ledc_conf.intr_type  = LEDC_INTR_DISABLE;
        _ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
        _ledc_conf.timer_sel  = PULSE_TIMER;
        _ledc_conf.hpoint     = 0;
        ledc_channel_config(&_ledc_conf);

        _init = true;
    }

    return ESP_OK;
}

esp_err_t Pulse::deinit()
{
    return ESP_OK;
}

esp_err_t Pulse::set(uint16_t pulse_width_us)
{
    if (_init)
    {
        _duty = PULSE_DUTY(pulse_width_us);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)_channel, _duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)_channel);
    }

    return ESP_OK;
}
