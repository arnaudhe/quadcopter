#include <esp_attr.h>
#include <os/mutex.h>

Mutex::Mutex()
{
    _handle = xSemaphoreCreateMutex();
}

void IRAM_ATTR Mutex::lock()
{
    xSemaphoreTake(_handle, portMAX_DELAY);
}

void IRAM_ATTR Mutex::unlock()
{
    xSemaphoreGive(_handle);
}
