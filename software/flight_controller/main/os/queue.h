#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

typedef QueueHandle_t queue_handle_t;

template <class T>
class Queue
{

  private:

    QueueHandle_t _handle;

  public:

    Queue(int size);
    Queue(queue_handle_t &handle);

    bool push(T&);
    bool pop(T&);
    void empty();
};

template <typename T>
inline Queue<T>::Queue(int size)
{
    _handle = xQueueCreate(size, sizeof(T));
}


template <typename T>
inline Queue<T>::Queue(queue_handle_t &handle)
{
    _handle = handle;
}


template <typename T>
inline bool Queue<T>::push(T& item)
{
    return (xQueueSend(_handle, (void *)&item, (portTickType)0) == pdPASS);
}


template <typename T>
inline bool Queue<T>::pop(T& item)
{
    return (xQueueReceive(_handle, (void *)&item, (portTickType)portMAX_DELAY) == pdPASS);
}

template <typename T>
inline void Queue<T>::empty()
{
    xQueueReset(_handle);
}