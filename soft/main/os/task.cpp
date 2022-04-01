#include <os/task.h>
#include <hal/log.h>

Task::Task(string name, Task::Priority priority, int stack_size, bool auto_start):
    _auto_start(auto_start),
    _name(name)
{
    if (priority <= Task::MEDIUM)
    {
        xTaskCreatePinnedToCore(Task::run_static, name.c_str(), stack_size, this, (UBaseType_t)priority, &_handle, 1);
    }
    else
    {
        xTaskCreatePinnedToCore(Task::run_static, name.c_str(), stack_size, this, (UBaseType_t)priority, &_handle, 0);
    }
}

void Task::run_static(void * pvParameters)
{
    Task * task = (Task *)pvParameters;

    if (!task->_auto_start)
    {
        LOG_INFO("Waiting to start %s task", task->_name.c_str());
        vTaskSuspend(task->_handle);
    }

    LOG_INFO("Entering %s task", task->_name.c_str());
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