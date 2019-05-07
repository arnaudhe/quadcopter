#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

template <class T>
class Queue
{

  private:

    QueueHandle_t _handle;

  public:

    Queue(int size);

    bool push(T&);
    bool pop(T&);
};
