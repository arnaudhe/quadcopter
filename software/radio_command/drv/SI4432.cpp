#include <drv/SI4432.hpp>
#include <drv/SI4432_defs.h>
#include <drv/SI4432_conf.h>
#include <stdio.h>
#include <unistd.h>
#include <platform.hpp>
#include <iostream>

Si4432::Si4432(void) :
    _spi(), _irq_gpio(PLATFORM_SI4432_IRQ, Gpio::INPUT)
{
    _spi.open(PLATFORM_SI4432_SPI, Spi::MODE_0, PLATFORM_SI4432_SPI_SPEED);

    _tx_done        = false;
    _rx_done        = false;
    _valid_preamble = false;
    _valid_sync     = false;

    this->reset();
    this->start_rx();
}

bool Si4432::write_config(void)
{
    this->write_modem_config();
    this->write_gpio_config();
    this->write_irq_config();
    this->write_packet_handler_config();
    this->set_frequency(SI4432_MODEM_FREQ);
    this->set_frequency_deviation(SI4432_MODEM_DEVIATION);
    this->set_data_rate(SI4432_MODEM_DATARATE);
    this->set_tx_power(SI4432_MODEM_TX_POWER);

    return true;
}

bool Si4432::read_status(uint8_t * status)
{
    return _spi.read_register(SI4432_REG_DEV_STATUS, status);
}

bool Si4432::reset(void)
{
    uint8_t irq2;

    _spi.write_register(SI4432_REG_STATE, SI4432_OPERATION_MODE_RESET);

    usleep(1000 * SI4432_STARTUP_TIME);

    do
    {
        usleep(1000 * 5);
        _spi.read_register(SI4432_REG_IRQ_STATUS_2, &irq2);
    } while ((irq2 & SI4432_IRQ2_CHIP_READY) == 0);

    return this->write_config();
}

bool Si4432::write_modem_config(void)
{
    /* Load capacitance */
    _spi.write_register(SI4432_REG_CRYSTAL_LOAD_CAPACITANCE, 0x7F);

    /* filters */
    _spi.write_register(SI4432_REG_IF_FILTER_BW, 0x9A);
    _spi.write_register(SI4432_REG_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x40);

    /* Modem low-level config */
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_OVERSAMPLING, 0x2C);
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_OFFSET2, 0x20);
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_OFFSET1, 0x36);
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_OFFSET0, 0x9D);
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN1, 00);
    _spi.write_register(SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN0, 0x39);

    /* Write config registers */
    _spi.write_register(SI4432_REG_MODULATION_MODE1, SI4432_TX_DATARATE_SCALE | SI4432_MODEM_ENCODING);
    _spi.write_register(SI4432_REG_MODULATION_MODE2, SI4432_MODEM_SOURCE | SI4432_MODEM_TYPE);

    return true;
}

bool Si4432::write_gpio_config(void)
{
    /* GPIO0 & GPIO1 - TX/RX switch */
    _spi.write_register(SI4432_REG_GPIO0_CONF, SI4432_GPIO0_CONFIG);
    _spi.write_register(SI4432_REG_GPIO1_CONF, SI4432_GPIO1_CONFIG);

    /* GPIO2 - IRQ */
    _spi.write_register(SI4432_REG_GPIO2_CONF, SI4432_GPIO2_CONFIG);

    return true;
}

bool Si4432::write_packet_handler_config(void)
{
    _spi.write_register(SI4432_REG_DATA_ACCESS_CONTROL, SI4432_PH_PATH | SI4432_PH_CRC | SI4432_PH_CRC_TYPE);
    _spi.write_register(SI4432_REG_HEADER_CONTROL2, SI4432_PH_HEADER_LENGTH | SI4432_PH_SYNC_LENGTH);
    _spi.write_register(SI4432_REG_PREAMBLE_LENGTH, SI4432_PH_PREAMBLE_LENGTH);
    _spi.write_register(SI4432_REG_PREAMBLE_DETECTION, (SI4432_PH_PREAMBLE_DETECT << 3) | 0x2);

    this->set_sync_bytes((uint8_t *)SI4432_PH_SYNC_BYTES, SI4432_PH_SYNC_LENGTH);

    return true;
}

bool Si4432::write_irq_config(void)
{
    this->clear_irq();

    _spi.write_register(SI4432_REG_IRQ_ENABLE_1, SI4432_IRQ_ENABLE_1);
    _spi.write_register(SI4432_REG_IRQ_ENABLE_2, SI4432_IRQ_ENABLE_2);

    return true;
}

bool Si4432::set_frequency(float freq)
{
    uint8_t  regs[3];
    uint32_t fb = 0;
    uint16_t fc = 0;
    uint8_t  hbsel = 0b01000000;
    uint32_t F = freq * 1000;

    if (F < 480000)
    {
        fb = (F - 240000);
    }
    else
    {
        fb = (F - 480000)/2;
        hbsel |= 0b00100000;
    }

    fc = (uint16_t) ((F/(1 + (F > 480000 ? 1 : 0)) - 240000 - (fb/10000) * 10000 ) * 100000 / 15625);

    regs[0] = hbsel | ((uint8_t)(fb / 10000) & 0b00011111);
    regs[1] = (uint8_t) ((fc >> 8) & 0xff);
    regs[2] = (uint8_t) (fc & 0xff);

    _spi.write_register(SI4432_REG_FREQ_BAND_SELECT, regs[0]);
    _spi.write_register(SI4432_REG_FREQCARRIER_H,    regs[1]);
    _spi.write_register(SI4432_REG_FREQCARRIER_L,    regs[2]);

    return true;
}

bool Si4432::set_data_rate(uint16_t datarate)
{
    uint8_t regs[2];
    uint64_t val = (uint64_t)datarate << 21;

    val /= 1000000;
    regs[0] = (uint8_t) ((val >> 8) & 0xff);
    regs[1] = (uint8_t) (val & 0xff);

    _spi.write_register(SI4432_REG_TX_DATARATE1, regs[0]);
    _spi.write_register(SI4432_REG_TX_DATARATE0, regs[1]);

    return true;
}

bool Si4432::set_frequency_deviation(uint32_t deviation)
{
    return _spi.write_register(SI4432_REG_FREQ_DEVIATION, (uint8_t) (deviation / 625));
}

bool Si4432::set_tx_power(SI4432_TX_POWER tx_power)
{
    return _spi.write_register(SI4432_REG_TX_POWER, 0x18 | tx_power);
}

bool Si4432::set_sync_bytes(uint8_t * bytes, uint8_t len)
{
    if (len <= 4)
    {
        return _spi.write_registers(SI4432_REG_SYNC_WORD3, len, bytes);
    }
    else
    {
        return false;
    }
}

bool Si4432::send_packet(uint8_t * packet, uint8_t length)
{
    this->clear_tx_fifo();
    this->clear_irq();
    _spi.write_register(SI4432_REG_TX_PKT_LEN, length);
    _spi.write_registers(SI4432_REG_FIFO, length, packet);
    return this->start_tx();
}

bool Si4432::receive_packet(uint8_t * packet, uint8_t * length)
{
    /* Set no data received as default result */
    *length = 0;

    /* Check if IAR is asserted */
    if (_irq_gpio.read() == 0)
    {
        this->read_irq();

        if (_rx_done)
        {
            /* If packet received, exit rx mode untill the packet is read out. Keep PLL on for faster reaction */
            _spi.write_register(SI4432_REG_STATE, SI4432_OPERATION_MODE_TUNE | SI4432_OPERATION_MODE_READY);

            /* Retrieve received packet from FIFO */
            _spi.read_register(SI4432_REG_RX_PKT_LENGTH, length);
            if (*length > 0)
            {
                std::cout << "Received packet, length " << length << std::endl;
                _spi.read_registers(SI4432_REG_FIFO, *length, packet);
            }

            /* Clear rx FIFO as it may contain any residual bytes */
            this->clear_rx_fifo();

            /* Go back to rx */
            _spi.write_register(SI4432_REG_STATE, SI4432_OPERATION_MODE_RX | SI4432_OPERATION_MODE_READY);
        }

        /* Clear all the flags */
        _valid_preamble = false;
        _valid_sync = false;
        _rx_done = false;
    }

    return true;
}

bool Si4432::start_tx(void)
{
    uint8_t time = 0;

    _spi.write_register(SI4432_REG_STATE, SI4432_OPERATION_MODE_TX);

    while (!_tx_done)
    {
        if (_irq_gpio.read() == false)
        {
            this->read_irq();
        }

        usleep(1000 * 5);
        time += 5;

        if (time > 50)
        {
            std::cerr << "tx timed out" << std::endl;
            this->reset();
            break;
        }
    }

    std::cout << "tx done" << std::endl;

    this->clear_irq();
    this->start_rx();

    return true;
}

bool Si4432::start_rx(void)
{
    this->clear_irq();
    _spi.write_register(SI4432_REG_STATE, SI4432_OPERATION_MODE_RX | SI4432_OPERATION_MODE_READY);
    return true;
}

bool Si4432::clear_tx_fifo(void)
{
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x1);
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x0);

    return true;
}

bool Si4432::clear_rx_fifo(void)
{
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x2);
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x0);

    return true;
}

bool Si4432::clear_both_fifo(void)
{
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x3);
    _spi.write_register(SI4432_REG_OPERATION_CONTROL, 0x0);

    return true;
}

bool Si4432::read_irq(void)
{
    uint8_t irq1, irq2;

    /* Read all IRQs to clear them */
    _spi.read_register(SI4432_REG_IRQ_STATUS_1, &irq1);
    _spi.read_register(SI4432_REG_IRQ_STATUS_2, &irq2);

    /* Extract events we are working with */
    _tx_done = (bool)(irq1 & SI4432_IRQ1_PACKET_SENT);
    _rx_done = (bool)(irq1 & SI4432_IRQ1_VALID_PACKET_RECEIVED);

    _valid_preamble = (bool)(irq2 & SI4432_IRQ2_VALID_PREAMBLE_DETECT);
    _valid_sync     = (bool)(irq2 & SI4432_IRQ2_SYNC_WORD_DETECT);

    return true;
}

bool Si4432::clear_irq(void)
{
    this->read_irq();

    /* Force events to be cleared */
    _tx_done        = false;
    _rx_done        = false;
    _valid_preamble = false;
    _valid_sync     = false;

    return true;
}

bool Si4432::read_rssi(int * rssi)
{
    bool ret;
    uint8_t rssi_lsb;
    
    ret = _spi.read_register(SI4432_REG_RSSI, &rssi_lsb);

    *rssi = ((int)rssi_lsb / 2) - 130;

    return ret;
}
