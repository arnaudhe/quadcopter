#include <hal/motor.h>
#include <hal/log.h>

#define PULSE_MIN_VALUE_us         48
#define PULSE_MAX_VALUE_us         2047

Motor::Motor(int channel, int pin)
{
    _dshot = new DShotRMT();
    _dshot->install((gpio_num_t)pin, (rmt_channel_t)channel);

    LOG_INFO("RMT installed");
}

void Motor::arm(void)
{
    _dshot->init();
}

void Motor::beep(void)
{
    _dshot->beep();
}

void Motor::set_speed(float speed)
{
    uint16_t width = (uint16_t)(speed * (float)(PULSE_MAX_VALUE_us - PULSE_MIN_VALUE_us)) + PULSE_MIN_VALUE_us;

    _dshot->sendThrottle(width);
}

void Motor::set_zero(void)
{
    _dshot->sendThrottle(PULSE_MIN_VALUE_us);
}
