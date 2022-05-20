#pragma once

class Adc
{

private:

    int _channel;

public:

    Adc(int channel);

    int get_value(void);

};
