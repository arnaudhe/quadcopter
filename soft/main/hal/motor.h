#pragma once

class Motor
{

private:

    int _channel;
    int _pin;

public:

    Motor(int channel, int pin);

    void set_speed(float speed);

};