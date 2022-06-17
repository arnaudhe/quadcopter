#pragma once

#include <pthread.h>
#include <string>

class Task
{

private:

    pthread_t    _handle;
    std::string  _name;
    bool         _running;

    static void * run_static(void *);

public:

    Task(std::string name, bool auto_start = true);
    virtual ~Task() {}

    virtual void run() {}

    void start();

    void stop();

    bool running();

    static void delay_ms(int duration);
};