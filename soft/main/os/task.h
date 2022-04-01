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
        VERY_LOW  = configMAX_PRIORITIES - 6,
        LOW       = configMAX_PRIORITIES - 5,
        MEDIUM    = configMAX_PRIORITIES - 4,
        HIGH      = configMAX_PRIORITIES - 3,
        VERY_HIGH = configMAX_PRIORITIES - 2,
    };

  private:

    TaskHandle_t _handle;
    bool         _auto_start;
    string       _name;

    static void run_static(void *);

  public:

    Task(string name, Task::Priority priority, int stack_size = 2048, bool auto_start = true);
    virtual ~Task() {}

    virtual void run() {}

    void start();

    static void delay_ms(int duration);
};