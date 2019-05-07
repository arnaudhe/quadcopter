#include <os/semaphore.h>

Semaphore::Semaphore()
{
    _handle = xSemaphoreCreateBinary();
}

bool Semaphore::wait()
{
    return (xSemaphoreTake(_handle, portMAX_DELAY) == pdTRUE);
}

bool Semaphore::notify()
{
    xSemaphoreGive(_handle);
    return true;
}

bool Semaphore::notify_from_isr(bool &must_yield)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(_handle, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

void Semaphore::yield_from_isr()
{
    portYIELD_FROM_ISR();
}