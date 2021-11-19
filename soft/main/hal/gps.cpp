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
#include <functional>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <hal/log.h>
#include <os/task.h>
#include <utils/nmea_parser.h>

#include <hal/gps.h>


/****************************************************************************************************************************
    Class functions
****************************************************************************************************************************/

Gps::Gps(DataRessourcesRegistry * registry, uart_port_t uart_port, int rx_pin, int tx_pin) :
    Task("GPS", Task::Priority::LOW, 4608, true)
{
    uart_config_t uart_config = {
        .baud_rate              = 9600,
        .data_bits              = UART_DATA_8_BITS,
        .parity                 = UART_PARITY_DISABLE,
        .stop_bits              = UART_STOP_BITS_1,
        .flow_ctrl              = UART_HW_FLOWCTRL_DISABLE,
        .source_clk             = UART_SCLK_APB
    };

    uart_pin_config_t uart_pin_config = {
        .tx                     = tx_pin,
        .rx                     = rx_pin,
        .rts                    = UART_PIN_NO_CHANGE,
        .cts                    = UART_PIN_NO_CHANGE
    };

    _registry = registry;
    _new_sequence = false;
    _worker_enable = true;
    _uart = new Uart(uart_port, uart_config, uart_pin_config);

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
                switch (sentence.type)
                {
                    case NMEAParser::GGA:
                        _registry->internal_set<int>("sensors.gps.time.hour", sentence.gga.utc_hour);
                        _registry->internal_set<int>("sensors.gps.time.minute", sentence.gga.utc_hour);
                        _registry->internal_set<int>("sensors.gps.time.second", sentence.gga.utc_hour);
                        _registry->internal_set<float>("sensors.gps.time.millis", sentence.gga.utc_hour);
                        _registry->internal_set<double>("sensors.gps.position.latitude.deg", sentence.gga.latitude_deg);
                        _registry->internal_set<int>("sensors.gps.position.latitude.min", sentence.gga.latitude_min);
                        _registry->internal_set<float>("sensors.gps.position.latitude.sec", sentence.gga.latitude_sec);
                        _registry->internal_set<double>("sensors.gps.position.longitude.deg", sentence.gga.longitude_deg);
                        _registry->internal_set<int>("sensors.gps.position.longitude.min", sentence.gga.longitude_min);
                        _registry->internal_set<float>("sensors.gps.position.longitude.sec", sentence.gga.longitude_sec);
                        _registry->internal_set<string>("sensors.gps.status.module_status", ( sentence.gga.satellite_fix_status > 0 ? "fixed" : "unfixed"));
                        _registry->internal_set<int>("sensors.gps.status.fixed_satellite", sentence.gga.fixed_satellite);
                        _registry->internal_set<float>("sensors.gps.position.altitude.msl", sentence.gga.altitude_msl);
                        _registry->internal_set<float>("sensors.gps.position.altitude.geo", sentence.gga.geoidal_altitude_separation);
                        break;

                    case NMEAParser::RMC:
                        _registry->internal_set<int>("sensors.gps.time.hour", sentence.rmc.utc_hour);
                        _registry->internal_set<int>("sensors.gps.time.minute", sentence.rmc.utc_hour);
                        _registry->internal_set<int>("sensors.gps.time.second", sentence.rmc.utc_hour);
                        _registry->internal_set<float>("sensors.gps.time.millis", sentence.rmc.utc_hour);
                        _registry->internal_set<double>("sensors.gps.position.latitude.deg", sentence.rmc.latitude_deg);
                        _registry->internal_set<int>("sensors.gps.position.latitude.min", sentence.rmc.latitude_min);
                        _registry->internal_set<float>("sensors.gps.position.latitude.sec", sentence.rmc.latitude_sec);
                        _registry->internal_set<double>("sensors.gps.position.longitude.deg", sentence.rmc.longitude_deg);
                        _registry->internal_set<int>("sensors.gps.position.longitude.min", sentence.rmc.longitude_min);
                        _registry->internal_set<float>("sensors.gps.position.longitude.sec", sentence.rmc.longitude_sec);
                        _registry->internal_set<float>("sensors.gps.speed.ground_speed", sentence.rmc.speed_ms);
                        _registry->internal_set<int>("sensors.gps.date.day", sentence.rmc.utc_day);
                        _registry->internal_set<int>("sensors.gps.date.month", sentence.rmc.utc_month);
                        _registry->internal_set<int>("sensors.gps.date.year", sentence.rmc.utc_year);

                    default:
                        break;
                }
            }
            _new_sequence = false;
        } 
        Task::delay_ms(10);
    }
    LOG_INFO("Stopping worker task");
    vTaskDelete(NULL);
}

void Gps::start()
{
    uart_pattern_t pattern_config = {
        .pattern_chr = '\n',
        .chr_num     = 1,
        .chr_tout    = 9,
        .post_idle   = 0,
        .pre_idle    = 0,
    };

    _uart->enable_pattern_detect(pattern_config);
    _uart->register_pattern_detected_callback(std::bind(&Gps::parse, this, std::placeholders::_1, std::placeholders::_2));
    _uart->start_event_loop();
    Task::start();
}