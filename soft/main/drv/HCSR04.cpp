#include <drv/HCSR04.h>
#include <soc/gpio_reg.h>
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"
#include <drv/HCSR04_defs.h>
#include <esp_log.h>

HcSr04::HcSr04(int echo_pin, int trig_channel, int trig_pin)
{
    _echo_gpio  = new Gpio((gpio_num_t)echo_pin, Gpio::Direction::INPUT, false, false);
    _trig_pulse = new Pulse(HCSR04_TRIG_PULSE_WIDTH_us, trig_pin, trig_channel);
    _echo_timer = new Timer(TIMER_GROUP_1, TIMER_0, 0.02);

    _height = 0.0;

    _trig_pulse->init();
    _trig_pulse->set(HCSR04_TRIG_PULSE_WIDTH_us);
    _echo_gpio->enable_interrupt(Gpio::InterruptSource::BOTH_EDGE, std::bind(&HcSr04::echo_handler, this));
}

void HcSr04::echo_handler()
{
    if (_echo_gpio->read())
    {
        _echo_timer->reset();
        _echo_timer->start();
    }
    else
    {
        _echo_timer->stop();
        _height = _echo_timer->get_time() / HCSR04_SCALE_FACTOR;
    }
}

float HcSr04::height()
{
    return _height;
}