#pragma once

#include <periph/spi.hpp>
#include <periph/gpio.hpp>
#include <drv/SI4432_defs.h>

class Si4432
{

private:

    Spi         _spi;
    Gpio        _irq_gpio;
    bool        _tx_done;
    bool        _rx_done;
    bool        _valid_preamble;
    bool        _valid_sync;

public:

    Si4432(void);

    bool reset(void);

    bool write_config(void);

    bool read_status(uint8_t * status);

    bool write_modem_config(void);

    bool write_gpio_config(void);

    bool write_packet_handler_config(void);

    bool write_irq_config(void);

    bool set_frequency(float freq);

    bool set_data_rate(uint16_t datarate);

    bool set_frequency_deviation(uint32_t deviation);

    bool set_tx_power(SI4432_TX_POWER tx_power);

    bool set_sync_bytes(uint8_t * bytes, uint8_t len);

    bool send_packet(uint8_t * buf, uint8_t length);

    bool receive_packet(uint8_t * packet, uint8_t * length);

    bool start_tx(void);

    bool start_rx(void);

    bool clear_tx_fifo(void);

    bool clear_rx_fifo(void);

    bool clear_both_fifo(void);

    bool read_irq(void);

    bool clear_irq(void);

    bool read_rssi(int * rssi);
};
