/****************************************************************************************************************************

NMEA controller class for ESP32 using IDF framework
nmea_controller.cpp

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
  18 Oct 2021  Use project custom logging.
  18 Oct 2021  Use project custom task.

****************************************************************************************************************************/


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <string.h>
#include <sstream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <hal/log.h>
#include <os/task.h>
#include <utils/nmea_parser.h>

#include <hal/gps.h>


/****************************************************************************************************************************
    Class functions
****************************************************************************************************************************/

Gps::Gps(DataRessourcesRegistry * registry) :
    Task("GPS", Task::Priority::LOW, 4608, true)
{
    _registry = registry;
    _new_sequence = false;
    _worker_enable = true;

    LOG_INFO("GPS object created.");
}


Gps::~Gps()
{
    _worker_enable = false;
    LOG_VERBOSE("GPS object destroyed.");
}


void Gps::parse(int len, std::string str)
{
    _nmea_sequence = str;
    _new_sequence = true;
}

void Gps::run()
{
    NMEAParser::ParsedSentence sentence;

    LOG_INFO("Worker task started");

    while (_worker_enable )
    {
        if ( _new_sequence )
        {
            LOG_VERBOSE("New sequence to parse");
            if (NMEAParser::parse(_nmea_sequence, &sentence))
            {
                LOG_DEBUG("Sequence parsed : %s", _nmea_sequence.c_str());
            }
            _new_sequence = false;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    LOG_INFO("Stoppinng worker task");
    vTaskDelete(NULL);
}
