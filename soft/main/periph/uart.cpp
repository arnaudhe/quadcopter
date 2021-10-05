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

****************************************************************************************************************************/


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#include "periph/uart.h"


/****************************************************************************************************************************
    Defines
****************************************************************************************************************************/

#define RX_BUF_SIZE (1024)
#define TX_BUF_SIZE (1024)
#define TAG "UART"


/****************************************************************************************************************************
    Class variables
****************************************************************************************************************************/

//static const char *TAG = "UART";
QueueHandle_t Uart::_uart_queue;


/****************************************************************************************************************************
    Class functions
****************************************************************************************************************************/

Uart::Uart(uart_port_t port, uart_config_t uart_config, uart_pin_config_t uart_pin_config)
{
    _port = port;
    _uart_config = uart_config;
    _uart_pin_config = uart_pin_config;
    _init = false;
    _data_available = false;
    _pattern_enable = false;
    ESP_LOGI(TAG, "Uart object created for port %d.", _port);
    esp_log_level_set(TAG, ESP_LOG_ERROR);
}


esp_err_t Uart::init()
{
    bool r = false;
    uart_param_config(_port, &_uart_config);
    uart_set_pin(_port, _uart_pin_config.tx, _uart_pin_config.rx, _uart_pin_config.cts, _uart_pin_config.rts);
    esp_err_t err = uart_driver_install(_port, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 20, &_uart_queue, 0);
    if ( err == ESP_OK )
    {
        r = true;
        _init = true;
        ESP_LOGV(TAG, "UART port %d opened.", _port);
    } else {
        ESP_LOGE(TAG, "Unable to open UART port %d.", _port);
    }
    return r;
}


void Uart::deinit()
{
    uart_driver_delete(_port);
    _init = false;
    ESP_LOGV(TAG, "UART port %d closed.", _port);
}


void Uart::write(uint8_t * data, uint8_t len)
{
    if ( _init )
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
    int readed_data_len = uart_read_bytes(_port, data, len, timeout / portTICK_RATE_MS);
    _data_available -= readed_data_len;
    return readed_data_len;  // The number of bytes read from UART FIFO
}


void Uart::start_pattern_detection(uart_pattern_t pattern_ptr, std::function<void(int, std::string)> callback)
{
    if ( _init )
    {
        esp_err_t err = uart_enable_pattern_det_baud_intr(_port,
                                                          pattern_ptr.pattern_chr,
                                                          pattern_ptr.chr_num,
                                                          pattern_ptr.chr_tout,
                                                          pattern_ptr.post_idle,
                                                          pattern_ptr.pre_idle);
        if ( err == ESP_OK )
        {
            _pattern_enable = true;
            _pattern_callback = callback;
            uart_pattern_queue_reset(_port, 16);
            ESP_LOGV(TAG, "Start detecting pattern on UART port %d.", _port);
            xTaskCreate(uart_event_worker_task, "uart_event_worker_task", 2048, this, 12, NULL);
        } else {
            ESP_LOGE(TAG, "Unable to start detecting pattern on UART port %d.", _port);
        }
    }
}


void Uart::uart_event_worker_task(void * instance_pointer)
{
    Uart * instance_ptr;
    instance_ptr = (Uart *)instance_pointer;
    
    ESP_LOGV(TAG, "Start event worker task on UART port %d.", instance_ptr->_port);

    uart_event_t event;
    size_t buffered_size;
    uint8_t * buffer = (uint8_t *) malloc(RX_BUF_SIZE);
    while ( instance_ptr->_pattern_enable )
    {
        if ( xQueueReceive(instance_ptr->_uart_queue, (void *)&event, (portTickType)portMAX_DELAY) )
        {
            memset(buffer, '\0', RX_BUF_SIZE);
            switch (event.type)
            {
                case UART_DATA:
                    instance_ptr->_data_available = event.size;
                    // a callback could be good here
                    break;
                case UART_BREAK:
                    ESP_LOGI(TAG, "UART break signal detected on port %d.", instance_ptr->_port);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "Full buffer detected. Flushing UART port %d.", instance_ptr->_port);
                    uart_flush_input(instance_ptr->_port);
                    xQueueReset(instance_ptr->_uart_queue);
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "HW FIFO overflow detected. Flushing UART port %d.", instance_ptr->_port);
                    uart_flush_input(instance_ptr->_port);
                    xQueueReset(instance_ptr->_uart_queue);
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGW(TAG, "UART frame error detected on port %d.", instance_ptr->_port);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "UART parity check error detected on port %d.", instance_ptr->_port);
                    break;
                case UART_DATA_BREAK:
                    ESP_LOGI(TAG, "UART TX data and break event detected on port %d.", instance_ptr->_port);
                    break;
                case UART_PATTERN_DET:
                {
                    ESP_LOGI(TAG, "UART pattern event detected on port %d.", instance_ptr->_port);
                    uart_get_buffered_data_len(instance_ptr->_port, &buffered_size);
                    int pos = uart_pattern_pop_pos(instance_ptr->_port);
                    if ( pos == -1 )
                    {
                        ESP_LOGW(TAG, "Full pattern position queue detected. Flushing UART port %d.", instance_ptr->_port);
                        uart_flush_input(instance_ptr->_port);
                    } else {
                        uart_read_bytes(instance_ptr->_port, buffer, pos, 100 / portTICK_PERIOD_MS);
                        uart_flush_input(instance_ptr->_port);
                        if (instance_ptr->_pattern_callback != NULL)
                        {
                            instance_ptr->_pattern_callback(buffered_size, std::string(reinterpret_cast<char*>(buffer)));
                        } else {
                            ESP_LOGI(TAG, "No callback provided for pattern detection on port %d.", instance_ptr->_port);
                        }
                    }
                    break;
                }
                case UART_EVENT_MAX:
                default:
                    ESP_LOGI(TAG, "Unhandled UART event deteted on port %d. Event id: %d", instance_ptr->_port, event.type);
                    break;
            }
        }
    }
    free(buffer);
    buffer = NULL;
    ESP_LOGI(TAG, "Pattern detection worker task stopped on port %d.", instance_ptr->_port);
    vTaskDelete(NULL);
}


void Uart::stop_pattern_detection()
{
    _pattern_enable = false;
    uart_disable_pattern_det_intr(_port);
    ESP_LOGI(TAG, "Stoppinng pattern detection worker task on port %d.", _port);
}


void Uart::flush()
{
    if (_init)
    {
        uart_flush(_port);
    }
}