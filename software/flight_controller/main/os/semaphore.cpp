#include <esp_attr.h>
#include <os/semaphore.h>

Semaphore::Semaphore()
{
    _handle = xSemaphoreCreateBinary();
}

bool IRAM_ATTR Semaphore::wait()
{
    return (xSemaphoreTake(_handle, portMAX_DELAY) == pdTRUE);
}

bool IRAM_ATTR Semaphore::notify()
{
    xSemaphoreGive(_handle);
    return true;
}

bool IRAM_ATTR Semaphore::notify_from_isr(bool &must_yield)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(_handle, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

void IRAM_ATTR Semaphore::yield_from_isr()
{
    portYIELD_FROM_ISR();
}