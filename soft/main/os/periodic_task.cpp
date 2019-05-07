#include <os/periodic_task.h>

map<TimerHandle_t, PeriodicTask *> PeriodicTask::tasks_map;

PeriodicTask::PeriodicTask(string name, int period, bool auto_start)
{
    _handle = xTimerCreate(name.c_str(), period / portTICK_PERIOD_MS, pdTRUE, ( void * ) 0, PeriodicTask::run_static);
    PeriodicTask::tasks_map[_handle] = this;
}

void PeriodicTask::run_static(TimerHandle_t handle)
{
    PeriodicTask::tasks_map[handle]->run();
}

void PeriodicTask::start()
{
    xTimerStart(_handle, 0);
}