#include <os/queue.h>

template <class T>
Queue<T>::Queue(int size)
{
    _handle = xQueueCreate(size, sizeof(T));
}

template <class T>
bool Queue<T>::push(T& item)
{
    return (xQueueSend(_handle, (void *)item, (TickType_t)0) == pdPASS);
}

template <class T>
bool Queue<T>::pop(T& item)
{
    T * tmp;
    bool res = (xQueueReceive(_handle, (void *)tmp, portMAX_DELAY) == pdPASS);
    if (res == true)
    {
        item = *tmp;
    }
    return res;
}