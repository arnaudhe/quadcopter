#pragma once

class Adc
{

private:

    int _channel;

    int _fd;

public:

    Adc(int channel);

    int get_value(void);

};
