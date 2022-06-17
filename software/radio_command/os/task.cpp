#include <os/task.hpp>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

Task::Task(std::string name, bool auto_start):
    _name(name), _running(false)
{
    if (auto_start)
    {
        start();
    }
}

void * Task::run_static(void * arg)
{
    Task * task = (Task *)arg;

    std::cout << "Start " << task->_name << std::endl;

    task->run();

    std::cout << "Stop " << task->_name << std::endl;

    return NULL;
}

void Task::start(void)
{
    if (_running == false)
    {
        _running = true;
        pthread_create(&_handle, NULL, Task::run_static, this);
    }
}

void Task::stop(void)
{
    if (_running)
    {
        // _running = false;
        pthread_join(_handle, NULL);
    }
}

bool Task::running(void)
{
    return _running;
}

void Task::delay_ms(int duration)
{
    usleep(duration * 1000);
}
