#pragma once

#include <driver/uart.h>

class Uart
{
    private: 

    /* Attributes */

    uart_port_t _port;       ///< i2c port
    bool        _init;

  public:

    /* Constructors */

    Uart(uart_port_t port);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    void deinit();

    void write(uint8_t * data, uint8_t len);

    int read(uint8_t * data, uint32_t len, uint32_t timeout);
};