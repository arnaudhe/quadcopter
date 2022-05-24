#include "adc.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <string.h>

Adc::Adc(int channel)
{
    _channel = channel;
}

int Adc::get_value(void)
{
    std::ifstream   indata;
    int             value;

    indata.open((std::string("/sys/bus/iio/devices/iio:device0/in_voltage") + std::to_string(_channel) + std::string("_raw")).c_str());
    if (!indata)
    {
        std::cerr << "Error: file could not be opened :" << strerror(errno) << std::endl;
        return 0;
    }

    do
    {
        indata >> value;
    } while (!indata.eof());

    indata.close();

    return value;
}
