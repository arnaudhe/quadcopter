#pragma once

#include <periph/gpio.h>
#include <periph/pulse.h>
#include <periph/timer.h>

#include <os/task.h>
#include <os/semaphore.h>
#include <os/mutex.h>

class HcSr04 : public Task
{

  private:

    Gpio        * _echo_gpio;
    Pulse       * _trig_pulse;
    Timer       * _echo_timer;
    Semaphore     _sem_rising;
    Semaphore     _sem_falling;
    Mutex         _mutex;

    float _height;

    void run(void);

    void echo_handler();

  public:

    HcSr04(int echo_pin, int trig_channel, int trig_pin);

    float height();
};