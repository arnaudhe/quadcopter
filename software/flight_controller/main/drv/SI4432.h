#pragma once

#include <periph/spi.h>
#include <periph/gpio.h>
#include <drv/SI4432_defs.h>

class Si4432
{

private:

    SPIDevice * _spi;
    Gpio      * _irq_gpio;
    bool        _tx_done;
    bool        _rx_done;
    bool        _valid_preamble;
    bool        _valid_sync;

public:

    Si4432(SPIHost * host);

    esp_err_t reset(void);

    esp_err_t write_config(void);

    esp_err_t read_status(uint8_t * status);

    esp_err_t write_modem_config(void);

    esp_err_t write_gpio_config(void);

    esp_err_t write_packet_handler_config(void);

    esp_err_t write_irq_config(void);

    esp_err_t set_frequency(float freq);

    esp_err_t set_data_rate(uint16_t datarate);

    esp_err_t set_frequency_deviation(uint32_t deviation);

    esp_err_t set_tx_power(SI4432_TX_POWER tx_power);

    esp_err_t set_sync_bytes(uint8_t * bytes, uint8_t len);

    esp_err_t send_packet(uint8_t * buf, uint8_t length);

    esp_err_t receive_packet(uint8_t * packet, uint8_t * length);

    esp_err_t start_tx(void);

    esp_err_t start_rx(void);

    esp_err_t clear_tx_fifo(void);

    esp_err_t clear_rx_fifo(void);

    esp_err_t clear_both_fifo(void);

    esp_err_t read_irq(void);

    esp_err_t clear_irq(void);

    esp_err_t read_rssi(int * rssi);
};