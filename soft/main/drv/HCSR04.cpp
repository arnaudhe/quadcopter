#include <drv/HCSR04.h>
#include <soc/gpio_reg.h>
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"
#include <drv/HCSR04_defs.h>
#include <hal/log.h>

HcSr04::HcSr04(int echo_pin, int trig_channel, int trig_pin):
    Task("HCSR04", Task::Priority::HIGH, 2048, false),
    _sem_rising(),
    _sem_falling(),
    _mutex()
{
    _echo_gpio  = new Gpio(echo_pin, Gpio::Direction::INPUT, false, false);
    _trig_pulse = new Pulse(HCSR04_TRIG_PULSE_WIDTH_us, trig_pin, trig_channel);
    _echo_timer = new Timer(TIMER_GROUP_1, TIMER_0, 0.02);

    _height = 0.0;

    _trig_pulse->init();
    _trig_pulse->set(HCSR04_TRIG_PULSE_WIDTH_us);
    _echo_gpio->enable_interrupt(Gpio::InterruptSource::BOTH_EDGE, std::bind(&HcSr04::echo_handler, this));

    start();
}

void HcSr04::echo_handler()
{
    bool must_yield;

    if (_echo_gpio->read())
    {
        _echo_timer->reset();
        _echo_timer->start();
    }
    else
    {
        _echo_timer->stop();
        _sem_falling.notify_from_isr(must_yield);
    }

    if (must_yield)
    {
        Semaphore::yield_from_isr();
    }
}

void HcSr04::run()
{
    float tmp;
    while(1)
    {
        _sem_falling.wait();
        tmp = _echo_timer->get_time() / HCSR04_SCALE_FACTOR;
        _mutex.lock();
        _height = tmp;
        _mutex.unlock();
    }
}

float HcSr04::height()
{
    float copy;

    _mutex.lock();
    copy = _height;
    _mutex.unlock();

    return copy;
}