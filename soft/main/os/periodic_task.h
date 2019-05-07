#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/timers.h>

#include <string>
#include <map>

using namespace std;

class PeriodicTask
{

  private:

    TimerHandle_t   _handle;
    bool            _auto_start;

    static void run_static(void *);

  public:

    static map<TimerHandle_t, PeriodicTask *> tasks_map;

    PeriodicTask(string name, int period, bool auto_start = true);
    virtual ~PeriodicTask() {}

    virtual void run() {}

    void start();
};