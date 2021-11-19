/****************************************************************************************************************************

UART class for ESP32 using IDF framework
uart.cpp

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
  05 Oct 2021  Use project custom logging.
  06 Oct 2021  Use project custom task.
  09 Oct 2021  Use project custom queue.

****************************************************************************************************************************/


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <string.h>

#include "driver/uart.h"
#include "esp_err.h"

#include "hal/log.h"
#include "os/task.h"
#include "os/queue.h"
#include "freertos/queue.h"

#include "periph/uart.h"


/****************************************************************************************************************************
    Defines
****************************************************************************************************************************/

#define RX_BUF_SIZE (1024)
#define TX_BUF_SIZE (1024)
#define QUEUE_SIZE  (20)


/****************************************************************************************************************************
    Class functions
****************************************************************************************************************************/

Uart::Uart(uart_port_t port, uart_config_t uart_config, uart_pin_config_t uart_pin_config) :
    Task("UART #" + port, Task::Priority::MEDIUM, 3072, false)
{
    _port = port;
    _uart_config = uart_config;
    _uart_pin_config = uart_pin_config;
    _open = false;
    _data_available = false;
    _event_enable = false;
    _data_in_callback = NULL;
    _pattern_callback = NULL;
    LOG_INFO("Uart object created for port %d.", _port);

    uart_param_config(_port, &_uart_config);
    uart_set_pin(_port, _uart_pin_config.tx, _uart_pin_config.rx, _uart_pin_config.cts, _uart_pin_config.rts);
    esp_err_t err = uart_driver_install(_port, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, QUEUE_SIZE, &_uart_queue_handle, 0);
    if ( err == ESP_OK )
    {
        _open = true;
        if ( _uart_queue_handle != NULL )
        {
            _uart_event_queue = new Queue<uart_event_t>(_uart_queue_handle);
        }
        else
        {
            LOG_ERROR("Failed to create UART event queue on port %d.", _port);
        }
        LOG_VERBOSE("UART port %d opened.", _port);
    }
    else
    {
        LOG_ERROR("Unable to open UART port %d.", _port);
    }
}

Uart::~Uart()
{
    if ( _open )
    {
        close();
    }
    if ( _uart_event_queue != NULL )
    {
        delete _uart_event_queue;
    }

    LOG_VERBOSE("UART object destroyed for port %d.", _port);
}


void Uart::close()
{
    uart_driver_delete(_port);
    _open = false;
    LOG_VERBOSE("UART port %d closed.", _port);
}


void Uart::write(uint8_t * data, uint8_t len)
{
    if ( _open )
    {
        uart_write_bytes(_port, (const char *)data, len);
    }
}


int Uart::get_data_available()
{
    return _data_available;
}


int Uart::read(uint8_t * data, uint32_t len, uint32_t timeout)
{
    int read_data_len = uart_read_bytes(_port, data, len, timeout / portTICK_RATE_MS);
    _data_available -= read_data_len;
    return read_data_len; // The number of bytes read from UART FIFO
}


void Uart::start_event_loop()
{
    _event_enable = true;
    start();
}


void Uart::stop_event_loop()
{
    _event_enable = false;
}


void  Uart::register_data_in_callback(std::function<void(int)> callback)
{
    _data_in_callback = callback;
}


void  Uart::register_pattern_detected_callback(std::function<void(int, std::string)> callback)
{
    _pattern_callback = callback;
}


void Uart::enable_pattern_detect(uart_pattern_t pattern)
{
    if ( _open )
    {
        esp_err_t err = uart_enable_pattern_det_baud_intr(_port,
                                                          pattern.pattern_chr,
                                                          pattern.chr_num,
                                                          pattern.chr_tout,
                                                          pattern.post_idle,
                                                          pattern.pre_idle);
        if ( err == ESP_OK )
        {
            uart_pattern_queue_reset(_port, QUEUE_SIZE/2);
        }
        else
        {
            LOG_ERROR("Unable to configure pattern detection on UART port %d.", _port);
        }
    }
}


void Uart::disable_pattern_detect()
{
    if ( _open )
    {
        esp_err_t err = uart_disable_pattern_det_intr(_port);
        if ( err != ESP_OK )
        {
            LOG_ERROR("Unable to disable pattern detecttion on UART port %d.", _port);
        }
    }
}


void Uart::run()
{
    LOG_VERBOSE("Start event worker task on UART port %d.", _port);

    uart_event_t * event = new uart_event_t();
    size_t         buffered_size;
    uint8_t      * buffer = (uint8_t *) malloc(RX_BUF_SIZE);
    uint8_t        dummy;

    while ( _event_enable )
    {
        if ( _uart_event_queue->pop(*event) )
        {
            memset(buffer, '\0', RX_BUF_SIZE);
            switch (event->type)
            {
                case UART_DATA:
                    _data_available = event->size;
                    if ( _data_in_callback != NULL)
                    {
                        _data_in_callback(event->size);
                    }
                    break;
                case UART_BREAK:
                    LOG_VERBOSE("UART break signal detected on port %d.", _port);
                    break;
                case UART_BUFFER_FULL:
                    LOG_WARNING("Full buffer detected. Flushing UART port %d.", _port);
                    uart_flush_input(_port);
                    _uart_event_queue->empty();
                    break;
                case UART_FIFO_OVF:
                    LOG_WARNING("HW FIFO overflow detected. Flushing UART port %d.", _port);
                    uart_flush_input(_port);
                    _uart_event_queue->empty();
                    break;
                case UART_FRAME_ERR:
                    LOG_WARNING("UART frame error detected on port %d.", _port);
                    break;
                case UART_PARITY_ERR:
                    LOG_WARNING("UART parity check error detected on port %d.", _port);
                    break;
                case UART_DATA_BREAK:
                    LOG_VERBOSE("UART TX data and break event detected on port %d.", _port);
                    break;
                case UART_PATTERN_DET:
                {
                    LOG_VERBOSE("UART pattern event detected on port %d.", _port);
                    uart_get_buffered_data_len(_port, &buffered_size);
                    int pos = uart_pattern_pop_pos(_port);
                    if ( pos == -1 )
                    {
                        LOG_WARNING("Full pattern position queue detected. Flushing UART port %d.", _port);
                        uart_flush_input(_port);
                    }
                    else
                    {
                        uart_read_bytes(_port, buffer, pos, 100 / portTICK_PERIOD_MS);
                        uart_read_bytes(_port, &dummy, 1,   100 / portTICK_PERIOD_MS);  // Read to pattern character to dummy
                        if ( _pattern_callback != NULL )
                        {
                            _pattern_callback(buffered_size, std::string(reinterpret_cast<char*>(buffer)));
                        }
                        else
                        {
                            LOG_WARNING("No callback provided for pattern detection on port %d.", _port);
                        }
                    }
                    break;
                }
                case UART_EVENT_MAX:
                default:
                    LOG_WARNING("Unhandled UART event deteted on port %d. Event id: %d", _port, event->type);
                    break;
            }
        }
    }
    free(buffer);
    buffer = NULL;
    LOG_INFO("Pattern detection worker task stopped on port %d.", _port);
}


void Uart::flush()
{
    if ( _open )
    {
        uart_flush(_port);
    }
}