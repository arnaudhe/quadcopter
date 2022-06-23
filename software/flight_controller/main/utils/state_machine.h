#pragma once

#include <functional>
#include <map>

#include <os/task.h>
#include <os/tick.h>

using state_function = std::function<void()>;

class StateMachine
{

private:

    std::map<int, state_function> _states;

    int _current_state;

    bool _first_run_in_state;

    Tick _state_tick;

protected:

    void add_state(int, state_function);

    void change_state(int);

    bool first_run_in_state();

public:

    StateMachine();

    void run(void);

    Tick time_in_state(void);

};
