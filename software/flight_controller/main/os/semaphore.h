#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class Semaphore
{

  private:

    SemaphoreHandle_t _handle;

  public:

    Semaphore();

    bool wait();

    bool notify();
    bool notify_from_isr(bool &must_yield);
    
    static void yield_from_isr();
};