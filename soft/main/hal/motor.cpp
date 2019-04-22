#include <hal/motor.h>
#include <esp_log.h>

#define PULSE_MIN_VALUE_us         1000
#define PULSE_MAX_VALUE_us         2000

Motor::Motor(int channel, int pin)
{
    _pulse = new Pulse(PULSE_MAX_VALUE_us, pin, channel);
    _pulse->init();
    set_speed(0.0);
}

void Motor::set_speed(float speed)
{
    uint16_t width = (uint16_t)(speed * (float)(PULSE_MAX_VALUE_us - PULSE_MIN_VALUE_us)) + PULSE_MIN_VALUE_us;
    ESP_LOGI("Motor", "command pulse width %d", width);
    _pulse->set(width);
}