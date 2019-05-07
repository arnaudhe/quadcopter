#include <os/task.h>
#include <hal/log.h>

Task::Task(string name, Task::Priority priority, int stack_size, bool auto_start):
    _auto_start(auto_start)
{
    xTaskCreate(Task::run_static, name.c_str(), stack_size, this, (UBaseType_t)priority, &_handle);
}

void Task::run_static(void * pvParameters)
{
    Task * task = (Task *)pvParameters;
    if (!task->_auto_start)
    {
        LOG_INFO("suspend task");
        vTaskSuspend(task->_handle);
    }
    LOG_INFO("resume task");
    task->run();
    vTaskDelete(NULL);
}

void Task::start()
{
    vTaskResume(_handle);
}

void Task::delay_ms(int duration)
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
}