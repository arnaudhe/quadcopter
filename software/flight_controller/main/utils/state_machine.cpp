#include "state_machine.h"

#include <hal/log.h>

StateMachine::StateMachine()
{
    _current_state = 0;
    _first_run_in_state = false;
    _state_tick = xTaskGetTickCount();
}

void StateMachine::add_state(int state, state_function state_function)
{
    _states[state] = state_function;
}

void StateMachine::change_state(int new_state)
{
    _current_state = new_state;
    _first_run_in_state = true;
    _state_tick = xTaskGetTickCount();
}

void StateMachine::run(void)
{
    _states[_current_state]();
}

bool StateMachine::first_run_in_state(void)
{
    bool ret = _first_run_in_state;
    _first_run_in_state = false;
    return ret;
}

uint32_t StateMachine::time_in_state(void)
{
    return xTaskGetTickCount() - _state_tick;
}
