#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>

#include <string>
#include <map>

#include <os/semaphore.h>

using namespace std;

class PeriodicTask
{

  private:

    TimerHandle_t     _timer;
    bool              _auto_start;
    string            _name;
    Semaphore       * _semaphore;
    int               _priority;
    int               _measure_count;
    TickType_t        _measure_tick;

    static void timer_function(TimerHandle_t timer);
    static void task_function(void * param);

  public:

    static map<TimerHandle_t, PeriodicTask *> timers_map;

    PeriodicTask(string name, int priority, int period, bool auto_start = true);
    virtual ~PeriodicTask() {}

    virtual void run() {}

    void start();
};