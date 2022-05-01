/****************************************************************************************************************************

NMEA controller class for ESP32 using IDF framework
nmea_controller.h

Created by Alexis Leprince (@lxslprnc) on October 17th 2021.
Copyright © 2021 Alexis Leprince (@lxslprnc). All rights reserved.

-----------------------------------------------------------------------------------------------------------------------------

Created to control a NMEA GPS module. This program mainly parses NMEA messages using a callback and then store position,
speed, date & time data in the project data resource registry. 
More details about NMEA messages here:
http://aprs.gids.nl/nmea/
Even if esp-idf framework works with many Espressif SoCs, this program has been designed for and tested on ESP-WROOM-32.
More details here:
https://github.com/espressif/esp-idf


MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----------------------------------------------------------------------------------------------------------------------------

Change Activity:
  Date         Description
  -----------  -------------
  17 Oct 2021  Created.

****************************************************************************************************************************/

#pragma once


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <stdlib.h>
#include <string>
#include <periph/uart.h>
#include <os/queue.h>

#include <data_model/data_ressources_registry.h>

using namespace std;

/****************************************************************************************************************************
    Defines
****************************************************************************************************************************/




/****************************************************************************************************************************
    Class declarations
****************************************************************************************************************************/

class Gps : public Task
{
    private:
        DataRessourcesRegistry *    _registry;
        Queue<char *>          *    _queue;
        bool                        _worker_enable;
        Uart                   *    _uart;

        void run();
        void parse(int len, std::string str);

    public:
        Gps(DataRessourcesRegistry * registry, uart_port_t uart_port, int rx_pin, int tx_pin);
        ~Gps();

        void start(void);
};