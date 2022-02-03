#include <drv/SI4432.h>
#include <drv/SI4432_defs.h>
#include <drv/SI4432_conf.h>
#include <platform.h>
#include <os/task.h>

#include <hal/log.h>

Si4432::Si4432(SPIHost * spi_host)
{
    uint8_t b;
    _spi = new SPIDevice(spi_host, PLATFORM_SI4432_SPI_MODE, PLATFORM_SI4432_SPI_FREQ, PLATFORM_SI4432_CS_PIN);

    _spi->read_byte(SI4432_REG_DEVICE_TYPE, &b);
    LOG_INFO("device type %02x", b);

    _spi->read_byte(SI4432_REG_VERSION, &b);
    LOG_INFO("version %02x", b);

    this->reset();
    this->write_modem_config();
    this->write_packet_handler_config();
    this->set_frequency(SI4432_MODEM_FREQ);
    this->set_frequency_deviation(SI4432_MODEM_DEVIATION);
    this->set_data_rate(SI4432_MODEM_DATARATE);
    this->set_tx_power(SI4432_MODEM_TX_POWER);
    this->set_sync_bytes((uint8_t *)SI4432_PH_SYNC_BYTES, SI4432_PH_SYNC_LENGTH);
}

esp_err_t Si4432::read_status(uint8_t * status)
{
    return _spi->read_byte(SI4432_REG_STATUS, status);
}

esp_err_t Si4432::reset(void)
{
    _spi->write_byte(SI4432_REG_CTRL1, SI4432_OPERATION_MODE_DEFAULT | 0b10000000);
    Task::delay_ms(20);

    return ESP_OK;
}

esp_err_t Si4432::write_modem_config(void)
{
    /* Write config registers */
    _spi->write_byte(SI4432_REG_CONF1, 0b00100000 | SI4432_MODEM_ENCODING);
    _spi->write_byte(SI4432_REG_CONF2, SI4432_MODEM_SOURCE | SI4432_MODEM_TYPE);

    /* Load capacitance */
    _spi->write_byte(0x09, 0xa5);

    /* GPIO0 & GPIO1 - TX/RX switch */
    _spi->write_byte(0x0b, 0x12);
    _spi->write_byte(0x0c, 0x15);

    return ESP_OK;
}

esp_err_t Si4432::write_packet_handler_config(void)
{
    _spi->write_byte(SI4432_REG_DATA_ACCESS_CONTROL, SI4432_PH_PATH | SI4432_PH_CRC | SI4432_PH_CRC_TYPE);
    _spi->write_byte(SI4432_REG_HEADER_CONTROL, SI4432_PH_HEADER_LENGTH | SI4432_PH_SYNC_LENGTH);
    _spi->write_byte(SI4432_REG_PREAMBULE_LENGTH, SI4432_PH_PREAMBULE_LENGTH);

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
    _spi->write_byte(SI4432_REG_FREQ_MSB, regs[1]);
    _spi->write_byte(SI4432_REG_FREQ_LSB, regs[2]);

    return ESP_OK;
}

esp_err_t Si4432::set_data_rate(uint16_t datarate)
{
    uint8_t regs[2];
    uint64_t val = (uint64_t)datarate << 21;

    val /= 1000000;
    regs[0] = (uint8_t) ((val >> 8) & 0xff);
    regs[1] = (uint8_t) (val & 0xff);

    _spi->write_byte(SI4432_REG_DATARATE_MSB, regs[0]);
    _spi->write_byte(SI4432_REG_DATARATE_LSB, regs[1]);

    return ESP_OK;
}

esp_err_t Si4432::set_frequency_deviation(uint32_t deviation)
{
    return _spi->write_byte(SI4432_REG_DEVIATION, (uint8_t) (deviation / 625));
}

esp_err_t Si4432::set_tx_power(SI4432_TX_POWER tx_power)
{
    return _spi->write_byte(SI4432_REG_TX_POWER, tx_power);
}

esp_err_t Si4432::set_sync_bytes(uint8_t * bytes, uint8_t len)
{
    if (len <= 4)
    {
        return _spi->write_bytes(SI4432_REG_SYNC3, len, bytes);
    }
    else
    {
        return ESP_FAIL;
    }
}

esp_err_t Si4432::send_packet(uint8_t * packet, uint8_t length)
{
    uint8_t b;
    this->clear_tx_fifo();
    _spi->read_byte(0x03, &b);
    _spi->read_byte(0x04, &b);
    _spi->write_byte(0x3e, length);
    _spi->write_bytes(SI4432_REG_FIFO_ACCESS, length, packet);
    return this->start_tx();
}

esp_err_t Si4432::start_tx(void)
{
    return _spi->write_byte(SI4432_REG_CTRL1, SI4432_OPERATION_MODE_DEFAULT | 0b00001000);
}

esp_err_t Si4432::clear_tx_fifo(void)
{
    _spi->write_byte(SI4432_REG_CTRL2, 1);
    _spi->write_byte(SI4432_REG_CTRL2, 0);

    return ESP_OK;
}

esp_err_t Si4432::force_recalibrate(void)
{
    _spi->write_byte(SI4432_REG_CALIBRATION, SI4432_FORCE_RACALIBRATION_VAL);
    Task::delay_ms(10);

    return ESP_OK;
}
