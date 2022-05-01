#include <os/periodic_task.h>
#include <esp_attr.h>
#include <hal/log.h>

map<TimerHandle_t, PeriodicTask *> PeriodicTask::timers_map;

PeriodicTask::PeriodicTask(string name, int priority, int stack_size, int period, bool auto_start)
{
    _name = name;
    _priority = priority;
    _stack_size = stack_size;
    _semaphore = new Semaphore();
    _measure_count = 0;
    _timer = xTimerCreate(name.c_str(), period / portTICK_PERIOD_MS, pdTRUE, (void *)0, PeriodicTask::timer_function);
    PeriodicTask::timers_map[_timer] = this;
}

void IRAM_ATTR PeriodicTask::timer_function(TimerHandle_t timer)
{
    PeriodicTask::timers_map[timer]->_semaphore->notify();
}

void IRAM_ATTR PeriodicTask::task_function(void * param)
{
    PeriodicTask *task = (PeriodicTask *)param;

    task->_measure_tick = xTaskGetTickCount();

    LOG_INFO("Entering %s loop", task->_name.c_str());

    while(1)
    {
        task->_semaphore->wait();
        task->_measure_count++;
        if ((xTaskGetTickCount() - task->_measure_tick) > 10000)
        {
            LOG_INFO("Task %s frequency %.1f Hz", task->_name.c_str(), (float)(task->_measure_count * 1000) / (xTaskGetTickCount() - task->_measure_tick));
            task->_measure_count = 0;
            task->_measure_tick = xTaskGetTickCount();
        }
        task->run();
    }
}

void IRAM_ATTR PeriodicTask::start()
{
    TaskHandle_t task;
    BaseType_t   ret;

    xTimerStart(this->_timer, 0);

    if (this->_priority <= Task::MEDIUM)
    {
        ret = xTaskCreatePinnedToCore(PeriodicTask::task_function, this->_name.c_str(), _stack_size, (void*)this, this->_priority, &task, 1);
    }
    else
    {
        ret = xTaskCreatePinnedToCore(PeriodicTask::task_function, this->_name.c_str(), _stack_size, (void*)this, this->_priority, &task, 0);
    }

    LOG_DEBUG("%s start exited with %d", _name.c_str(), ret);
}
