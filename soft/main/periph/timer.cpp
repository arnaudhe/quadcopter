#include <periph/timer.h>

#define TIMER_CLOCK_PERIOD        (1.0 / (float)TIMER_BASE_CLK)
#define TIMER_CLOCK_DIVIDER       2

typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

Timer::Timer(timer_group_t group, timer_idx_t idx, float period)
{
    timer_config_t config;

    config.alarm_en    = TIMER_ALARM_DIS;
    config.auto_reload = TIMER_AUTORELOAD_EN;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en  = TIMER_PAUSE;
    config.divider     = TIMER_CLOCK_DIVIDER;
    config.intr_type   = TIMER_INTR_LEVEL;

    _group = group;
    _idx   = idx;

    timer_init(group, idx, &config);
    timer_disable_intr(group, idx);
    timer_set_alarm_value(group, idx, (uint64_t)(period / (TIMER_BASE_CLK * TIMER_CLOCK_DIVIDER)));
    timer_start(group, idx);
}

void Timer::start()
{
    timer_start(_group, _idx);
}

void Timer::stop()
{
    timer_pause(_group, _idx);
}

float Timer::get_time()
{
    uint64_t counter;
    timer_get_counter_value(_group, _idx, &counter);
    return (float)counter * (TIMER_CLOCK_PERIOD * TIMER_CLOCK_DIVIDER);
}

void Timer::reset()
{
    timer_set_counter_value(_group, _idx, 0);
}

void IRAM_ATTR Timer::isr_handler(void * arg)
{
    Timer * timer = (Timer *)arg;
    // timer->_callback();

    TIMERG1.int_clr_timers.t0 = 1;
    
    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    TIMERG1.hw_timer[timer->_idx].config.alarm_en = TIMER_ALARM_EN;
}

void Timer::enable_interrupt(std::function<void()> callback)
{
    _callback = callback;
    timer_pause(_group, _idx);
    timer_set_alarm(_group, _idx, TIMER_ALARM_EN);
    timer_enable_intr(_group, _idx);
    timer_isr_register(_group, _idx, Timer::isr_handler, this, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(_group, _idx);
}