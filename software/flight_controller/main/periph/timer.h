#pragma once

#include <driver/timer.h>
#include <functional>
#include <esp_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

class Timer
{

  private:

    timer_group_t           _group;
    timer_idx_t             _idx;
    std::function<void()>   _callback;

    static void isr_handler(void * arg);

  public:

    Timer(timer_group_t group, timer_idx_t idx, float period);

    void start();

    void stop();

    float get_time();

    void reset();

    void enable_interrupt(std::function<void()>);

};