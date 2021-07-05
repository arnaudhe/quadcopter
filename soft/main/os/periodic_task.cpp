#include <os/periodic_task.h>

map<TimerHandle_t, PeriodicTask *> PeriodicTask::timers_map;

PeriodicTask::PeriodicTask(string name, int priority, int period, bool auto_start)
{
    _name = name;
    _priority = priority;
    _semaphore = new Semaphore();
    _timer = xTimerCreate(name.c_str(), period / portTICK_PERIOD_MS, pdTRUE, (void *)0, PeriodicTask::timer_function);
    PeriodicTask::timers_map[_timer] = this;
}

void PeriodicTask::timer_function(TimerHandle_t timer)
{
    PeriodicTask::timers_map[timer]->_semaphore->notify();
}

void PeriodicTask::task_function(void * param)
{
    PeriodicTask *task = (PeriodicTask *)param;

    while(1)
    {
        task->_semaphore->wait();
        task->run();
    }
}

void PeriodicTask::start()
{
    TaskHandle_t task;

    xTimerStart(this->_timer, 0);
    xTaskCreatePinnedToCore(PeriodicTask::task_function, this->_name.c_str(), 1024 * 16, (void*)this, this->_priority, &task, 0);
}
