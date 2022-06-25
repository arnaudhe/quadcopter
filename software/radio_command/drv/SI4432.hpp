#pragma once

#include <periph/spi.hpp>
#include <periph/gpio.hpp>
#include <drv/SI4432_defs.h>
#include <os/task.hpp>
#include <utils/state_machine.hpp>
#include <utils/byte_array.hpp>

class Si4432 : public Task, public StateMachine
{

private:

    enum State
    {
        RX,
        TX
    };

    Spi         _spi;
    Gpio        _irq_gpio;
    bool        _tx_done;
    bool        _rx_done;
    bool        _valid_preamble;
    bool        _valid_sync;
    int         _rx_rssi;
    ByteArray   _rx_packet;
    ByteArray   _tx_packet;
    Tick        _rx_tick;

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

    bool start_tx(void);

    bool stop_tx(void);

    bool start_rx(void);

    bool clear_tx_fifo(void);

    bool clear_rx_fifo(void);

    bool clear_both_fifo(void);

    bool read_irq(void);

    bool clear_irq(void);

    bool read_rssi(int * rssi);

    void state_tx(void);

    void state_rx(void);

    void run(void);

public:

    Si4432(void);

    bool send_packet(ByteArray packet);

    bool receive_packet(ByteArray &packet);
};
