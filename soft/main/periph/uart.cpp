#include <periph/uart.h>
#include <freertos/FreeRTOS.h>
#include <periph/uart_conf.h>

#define BUF_SIZE    1024

Uart::Uart(uart_port_t port)
{
    _port = port;
    _init = false;
}

esp_err_t Uart::init()
{
    uart_param_config(_port, &UART_CONF[_port]);
    uart_set_pin(_port, UART_PIN_CONF[_port].tx, UART_PIN_CONF[_port].rx, UART_PIN_CONF[_port].cts, UART_PIN_CONF[_port].rts);
    _init = true;
    return uart_driver_install(_port, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void Uart::deinit()
{
    uart_driver_delete(_port);
    _init = false;
}

void Uart::write(uint8_t * data, uint8_t len)
{
    if (_init)
    {
        uart_write_bytes(_port, (const char *)data, len);
    }
}

int Uart::read(uint8_t * data, uint32_t len, uint32_t timeout)
{
    return uart_read_bytes(_port, data, len, timeout / portTICK_RATE_MS);
}