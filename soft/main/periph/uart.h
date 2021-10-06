/****************************************************************************************************************************

UART class for ESP32 using IDF framework
uart.h

Created by Alexis Leprince (@lxslprnc) on October 3rd 2021.
Copyright © 2021 Alexis Leprince (@lxslprnc). All rights reserved.

-----------------------------------------------------------------------------------------------------------------------------

Created for the UART driver of the esp-idf framework:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html
This class handle opening & closing ports, reading & writing data on it, and it can trigger several RX UART events.
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
  03 Oct 2021  Created.

****************************************************************************************************************************/

#pragma once


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <functional>
#include <string.h>
#include <driver/uart.h>

#include "os/task.h"


/****************************************************************************************************************************
    Defines
****************************************************************************************************************************/

typedef struct
{
    int tx;
    int rx;
    int rts;
    int cts;
} uart_pin_config_t;

typedef struct // uart pattern detection conf
{
    char        pattern_chr; // character of the pattern
    uint8_t     chr_num; // number of the character
    int         chr_tout; // timeout of the interval between each pattern characters, *
    int         post_idle; // idle time after the last pattern character, *
    int         pre_idle; // idle time after the first pattern character, *
    // * unit is the baud-rate cycle you configured.
} uart_pattern_t;


/****************************************************************************************************************************
    Class declarations
****************************************************************************************************************************/

class Uart : public Task
{
    private:
        uart_port_t                             _port;
        uart_config_t                           _uart_config;
        uart_pin_config_t                       _uart_pin_config;
        bool                                    _open;
        int                                     _data_available;
        bool                                    _event_enable;
        static QueueHandle_t                    _uart_queue;
        std::function<void(int)>                _data_in_callback;
        std::function<void(int, std::string)>   _pattern_callback;

        void run();
        static void uart_event_worker_task(void * instance_pointer);

    public:
        Uart(uart_port_t port, uart_config_t uart_config, uart_pin_config_t uart_pin_config);
        ~Uart();

        void        close();
        void        write(uint8_t * data,
                          uint8_t len);
        int         get_data_available();
        int         read(uint8_t * data,
                         uint32_t len,
                         uint32_t timeout);
        void        start_uart_event();
        void        stop_uart_event();
        void        register_data_in_callback(std::function<void(int)> callback);
        void        register_pattern_detected_callback(std::function<void(int, std::string)> callback);
        void        enable_pattern_detect(uart_pattern_t pattern);
        void        disable_pattern_detect();
        void        flush();
};