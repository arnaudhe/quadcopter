#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/task.h>

#include <string>

using namespace std;

class Task
{

  public:

    enum Priority
    {
        VERY_LOW  = 0,
        LOW       = 1,
        MEDIUM    = 2,
        HIGH      = 3,
        VERY_HIGH = 4
    };

  private:

    TaskHandle_t _handle;
    bool         _auto_start;

    static void run_static(void *);

  public:

    Task(string name, Task::Priority priority, int stack_size = 2048, bool auto_start = true);
    virtual ~Task() {}

    virtual void run() {}

    void start();

    static void delay_ms(int duration);
};