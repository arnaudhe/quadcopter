#pragma once

#include <periph/spi.h>
#include <drv/SI4432_defs.h>

class Si4432
{

private:

    SPIDevice * _spi;

public:

    Si4432(SPIHost * host);

    esp_err_t reset(void);

    esp_err_t read_status(uint8_t * status);

    esp_err_t write_modem_config(void);

    esp_err_t write_packet_handler_config(void);

    esp_err_t set_frequency(float freq);

    esp_err_t set_data_rate(uint16_t datarate);

    esp_err_t set_frequency_deviation(uint32_t deviation);

    esp_err_t set_tx_power(SI4432_TX_POWER tx_power);

    esp_err_t set_sync_bytes(uint8_t * bytes, uint8_t len);

    esp_err_t send_packet(uint8_t * buf, uint8_t length);

    esp_err_t start_tx(void);

    esp_err_t clear_tx_fifo(void);

    esp_err_t force_recalibrate(void);
};
