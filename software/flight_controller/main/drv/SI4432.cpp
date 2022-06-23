#include <drv/SI4432.h>
#include <drv/SI4432_defs.h>
#include <drv/SI4432_conf.h>
#include <platform.h>
#include <os/task.h>
#include <os/periodic_task.h>

#include <hal/log.h>

Si4432::Si4432(SPIHost * spi_host) : PeriodicTask("Si4432", Task::Priority::MEDIUM, 3 * 1024, 50, false), StateMachine()
{
    uint8_t b;

    add_state(State::RX, std::bind(&Si4432::state_rx, this));
    add_state(State::TX, std::bind(&Si4432::state_tx, this));

    _tx_packet.clear();
    _rx_packet.clear();

    _spi = new SPIDevice(spi_host, PLATFORM_SI4432_SPI_MODE, PLATFORM_SI4432_SPI_FREQ, PLATFORM_SI4432_CS_PIN);
    _irq_gpio = new Gpio(PLATFORM_SI4432_IRQ_PIN, Gpio::INPUT, false, false);

    _irq_gpio->enable_interrupt(Gpio::FALLING_EDGE, std::bind(&Si4432::on_irq, this));

    _tx_done        = false;
    _rx_done        = false;
    _valid_preamble = false;
    _valid_sync     = false;

    _spi->read_byte(SI4432_REG_DEV_TYPE, &b);
    LOG_INFO("device type %02x", b);

    _spi->read_byte(SI4432_REG_DEV_VERSION, &b);
    LOG_INFO("version %02x", b);

    this->reset();

    this->start_rx();
}

void Si4432::on_irq(void)
{
    this->on_demand_run(true);
}

void Si4432::run(void)
{
    StateMachine::run();
}

void Si4432::state_tx(void)
{
    if (first_run_in_state())
    {
        LOG_VERBOSE("State TX");

        this->clear_tx_fifo();
        this->clear_irq();
        _spi->write_byte(SI4432_REG_TX_PKT_LEN, _tx_packet.length());
        _spi->write_bytes(SI4432_REG_FIFO, _tx_packet.length(), _tx_packet.data());
        this->start_tx();
        _tx_packet.clear();
    }

    /* Check if IRQ is asserted */
    if (_irq_gpio->read() == 0)
    {
        this->read_irq();

        if (_tx_done)
        {
            LOG_VERBOSE("TX done");
            this->clear_tx_fifo();
            change_state(State::RX);
        }

        /* Clear flag */
        _tx_done = false;
    }

    if (time_in_state() > 200)
    {
        LOG_WARNING("TX timeout");
        this->clear_tx_fifo();
        change_state(State::RX);
    }
}

void Si4432::state_rx(void)
{
    uint8_t rx_length;
    uint8_t rx_packet[64];

    if (first_run_in_state())
    {
        LOG_VERBOSE("State RX");
        this->clear_irq();
        this->start_rx();
    }

    /* Check if IRQ is asserted */
    if (_irq_gpio->read() == 0)
    {
        this->read_irq();

        if (_valid_preamble)
        {
            LOG_VERBOSE("Valid preamble");
        }

        if (_valid_sync)
        {
            LOG_VERBOSE("Sync word detected");
            this->read_rssi(&_rx_rssi);
        }

        if (_rx_done)
        {
            /* If packet received, exit rx mode untill the packet is read out. Keep PLL on for faster reaction */
            _spi->write_byte(SI4432_REG_STATE, SI4432_OPERATION_MODE_TUNE | SI4432_OPERATION_MODE_READY);

            /* Retrieve received packet from FIFO */
            _spi->read_byte(SI4432_REG_RX_PKT_LENGTH, &rx_length);
            if (rx_length > 0)
            {
                LOG_VERBOSE("Received packet, length %d, rssi %d dB", rx_length, _rx_rssi);
                _spi->read_bytes(SI4432_REG_FIFO, rx_length, rx_packet);
                _rx_packet.set_data(rx_packet, rx_length);
            }

            /* Clear rx FIFO as it may contain any residual bytes */
            this->clear_rx_fifo();

            /* Go back to rx */
            _spi->write_byte(SI4432_REG_STATE, SI4432_OPERATION_MODE_RX | SI4432_OPERATION_MODE_READY);
        }

        /* Clear all the flags */
        _valid_preamble = false;
        _valid_sync = false;
        _rx_done = false;
    }
    else if (_tx_packet.length())
    {
        change_state(State::TX);
    }
}

esp_err_t Si4432::write_config(void)
{
    this->write_modem_config();
    this->write_gpio_config();
    this->write_irq_config();
    this->write_packet_handler_config();
    this->set_frequency(SI4432_MODEM_FREQ);
    this->set_frequency_deviation(SI4432_MODEM_DEVIATION);
    this->set_data_rate(SI4432_MODEM_DATARATE);
    this->set_tx_power(SI4432_MODEM_TX_POWER);

    return ESP_OK;
}

esp_err_t Si4432::read_status(uint8_t * status)
{
    return _spi->read_byte(SI4432_REG_DEV_STATUS, status);
}

esp_err_t Si4432::reset(void)
{
    uint8_t irq2;

    _spi->write_byte(SI4432_REG_STATE, SI4432_OPERATION_MODE_RESET);

    Task::delay_ms(SI4432_STARTUP_TIME);

    do
    {
        Task::delay_ms(5);
        _spi->read_byte(SI4432_REG_IRQ_STATUS_2, &irq2);
    } while ((irq2 & SI4432_IRQ2_CHIP_READY) == 0);

    return this->write_config();
}

esp_err_t Si4432::write_modem_config(void)
{
    /* Load capacitance */
    _spi->write_byte(SI4432_REG_CRYSTAL_LOAD_CAPACITANCE, 0x7F);

    /* filters */
    _spi->write_byte(SI4432_REG_IF_FILTER_BW, 0x9A);
    _spi->write_byte(SI4432_REG_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x40);

    /* Modem low-level config */
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_OVERSAMPLING, 0x2C);
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_OFFSET2, 0x20);
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_OFFSET1, 0x36);
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_OFFSET0, 0x9D);
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN1, 00);
    _spi->write_byte(SI4432_REG_CLOCK_RECOVERY_TIMING_GAIN0, 0x39);

    /* Write config registers */
    _spi->write_byte(SI4432_REG_MODULATION_MODE1, SI4432_TX_DATARATE_SCALE | SI4432_MODEM_ENCODING);
    _spi->write_byte(SI4432_REG_MODULATION_MODE2, SI4432_MODEM_SOURCE | SI4432_MODEM_TYPE);

    return ESP_OK;
}

esp_err_t Si4432::write_gpio_config(void)
{
    /* GPIO0 & GPIO1 - TX/RX switch */
    _spi->write_byte(SI4432_REG_GPIO0_CONF, SI4432_GPIO0_CONFIG);
    _spi->write_byte(SI4432_REG_GPIO1_CONF, SI4432_GPIO1_CONFIG);

    /* GPIO2 - IRQ */
    _spi->write_byte(SI4432_REG_GPIO2_CONF, SI4432_GPIO2_CONFIG);

    return ESP_OK;
}

esp_err_t Si4432::write_packet_handler_config(void)
{
    _spi->write_byte(SI4432_REG_DATA_ACCESS_CONTROL, SI4432_PH_PATH | SI4432_PH_CRC | SI4432_PH_CRC_TYPE);
    _spi->write_byte(SI4432_REG_HEADER_CONTROL2, SI4432_PH_HEADER_LENGTH | SI4432_PH_SYNC_LENGTH);
    _spi->write_byte(SI4432_REG_PREAMBLE_LENGTH, SI4432_PH_PREAMBLE_LENGTH);
    _spi->write_byte(SI4432_REG_PREAMBLE_DETECTION, (SI4432_PH_PREAMBLE_DETECT << 3) | 0x2);

    this->set_sync_bytes((uint8_t *)SI4432_PH_SYNC_BYTES, SI4432_PH_SYNC_LENGTH);

    return ESP_OK;
}

esp_err_t Si4432::write_irq_config(void)
{
    this->clear_irq();

    _spi->write_byte(SI4432_REG_IRQ_ENABLE_1, SI4432_IRQ_ENABLE_1);
    _spi->write_byte(SI4432_REG_IRQ_ENABLE_2, SI4432_IRQ_ENABLE_2);

    return ESP_OK;
}

esp_err_t Si4432::set_frequency(float freq)
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

    _spi->write_byte(SI4432_REG_FREQ_BAND_SELECT, regs[0]);
    _spi->write_byte(SI4432_REG_FREQCARRIER_H,    regs[1]);
    _spi->write_byte(SI4432_REG_FREQCARRIER_L,    regs[2]);

    return ESP_OK;
}

esp_err_t Si4432::set_data_rate(uint16_t datarate)
{
    uint8_t regs[2];
    uint64_t val = (uint64_t)datarate << 21;

    val /= 1000000;
    regs[0] = (uint8_t) ((val >> 8) & 0xff);
    regs[1] = (uint8_t) (val & 0xff);

    _spi->write_byte(SI4432_REG_TX_DATARATE1, regs[0]);
    _spi->write_byte(SI4432_REG_TX_DATARATE0, regs[1]);

    return ESP_OK;
}

esp_err_t Si4432::set_frequency_deviation(uint32_t deviation)
{
    return _spi->write_byte(SI4432_REG_FREQ_DEVIATION, (uint8_t) (deviation / 625));
}

esp_err_t Si4432::set_tx_power(SI4432_TX_POWER tx_power)
{
    return _spi->write_byte(SI4432_REG_TX_POWER, 0x18 | tx_power);
}

esp_err_t Si4432::set_sync_bytes(uint8_t * bytes, uint8_t len)
{
    if (len <= 4)
    {
        return _spi->write_bytes(SI4432_REG_SYNC_WORD3, len, bytes);
    }
    else
    {
        return ESP_FAIL;
    }
}

esp_err_t Si4432::start_tx(void)
{
    return _spi->write_byte(SI4432_REG_STATE, SI4432_OPERATION_MODE_TX);
}

esp_err_t Si4432::start_rx(void)
{
    this->clear_irq();
    _spi->write_byte(SI4432_REG_STATE, SI4432_OPERATION_MODE_RX | SI4432_OPERATION_MODE_READY);
    return ESP_OK;
}

esp_err_t Si4432::clear_tx_fifo(void)
{
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x1);
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x0);

    return ESP_OK;
}

esp_err_t Si4432::clear_rx_fifo(void)
{
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x2);
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x0);

    return ESP_OK;
}

esp_err_t Si4432::clear_both_fifo(void)
{
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x3);
    _spi->write_byte(SI4432_REG_OPERATION_CONTROL, 0x0);

    return ESP_OK;
}

esp_err_t Si4432::read_irq(void)
{
    uint8_t irq1, irq2;

    /* Read all IRQs to clear them */
    _spi->read_byte(SI4432_REG_IRQ_STATUS_1, &irq1);
    _spi->read_byte(SI4432_REG_IRQ_STATUS_2, &irq2);

    /* Extract events we are working with */
    _tx_done = (bool)(irq1 & SI4432_IRQ1_PACKET_SENT);
    _rx_done = (bool)(irq1 & SI4432_IRQ1_VALID_PACKET_RECEIVED);

    _valid_preamble = (bool)(irq2 & SI4432_IRQ2_VALID_PREAMBLE_DETECT);
    _valid_sync     = (bool)(irq2 & SI4432_IRQ2_SYNC_WORD_DETECT);

    return ESP_OK;
}

esp_err_t Si4432::clear_irq(void)
{
    this->read_irq();

    /* Force events to be cleared */
    _tx_done        = false;
    _rx_done        = false;
    _valid_preamble = false;
    _valid_sync     = false;

    return ESP_OK;
}

esp_err_t Si4432::read_rssi(int * rssi)
{
    esp_err_t ret;
    uint8_t rssi_lsb;
    
    ret = _spi->read_byte(SI4432_REG_RSSI, &rssi_lsb);

    *rssi = ((int)rssi_lsb / 2) - 130;

    return ret;
}

bool Si4432::send_packet(ByteArray packet)
{
    if (_tx_packet.length() == 0)
    {
        _tx_packet = packet;
        return true;
    }
    else
    {
        return false;
    }
}

bool Si4432::receive_packet(ByteArray &packet, int &rssi)
{
    if (_rx_packet.length())
    {
        packet = _rx_packet;
        rssi = _rx_rssi;
        _rx_packet.clear();
        return true;
    }
    else
    {
        return false;
    }
}