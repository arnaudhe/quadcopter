#include <os/mutex.h>

Mutex::Mutex()
{
    _handle = xSemaphoreCreateMutex();
}

void Mutex::lock()
{
    xSemaphoreTake(_handle, portMAX_DELAY);
}

void Mutex::unlock()
{
    xSemaphoreGive(_handle);
}
