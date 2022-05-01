#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class Mutex
{

  private:

    SemaphoreHandle_t _handle;

  public:

    Mutex();

    void lock();
    void unlock();

};