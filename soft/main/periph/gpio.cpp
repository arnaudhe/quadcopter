#include <periph/gpio.h>

Gpio::Gpio(gpio_num_t pin, Gpio::Direction direction, bool pull_up, bool pull_down)
{
    _pin = pin;
    _direction = direction;

    gpio_config_t config;
    
    config.intr_type    = GPIO_INTR_DISABLE;
    config.mode         = direction == Gpio::Direction::INPUT ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;
    config.pull_up_en   = (gpio_pullup_t)pull_up;
    config.pull_down_en = (gpio_pulldown_t)pull_down;
    config.pin_bit_mask = (1 << pin);

    gpio_config(&config);
}

bool Gpio::read()
{
    return gpio_get_level(_pin);
}

void Gpio::write(bool level)
{
    gpio_set_level(_pin, (uint32_t)level);
}

void Gpio::enable_interrupt(Gpio::InterruptSource source, std::function<void()> callback)
{
    _callback = callback;
    gpio_install_isr_service(0);
    gpio_set_intr_type(_pin, (source == Gpio::InterruptSource::RISING_EDGE) ? GPIO_INTR_POSEDGE : ((source == Gpio::InterruptSource::FALLING_EDGE) ? GPIO_INTR_NEGEDGE : GPIO_INTR_ANYEDGE));
    gpio_isr_handler_add(_pin, &Gpio::isr_handler, this);
}

void IRAM_ATTR Gpio::isr_handler(void * arg)
{
    Gpio * gpio = (Gpio *)arg;
    gpio->_callback();
}