#pragma once

typedef enum
{
    GPIO_INPUT = 1,
    GPIO_OUTPUT = 0,
}gpio_direction_t;

class Gpio
{

public:

    enum Direction
    {
        INPUT  = 0,
        OUTPUT = 1,
    };

private:

    int                   pin;
    Direction             direction;
    volatile unsigned   * gpio_reg;

public:

    Gpio(int pin, Direction direction);

    bool read();

    void write(bool state);

};

