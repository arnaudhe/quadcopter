#include <periph/spi.h>
#include <stdio.h>
#include <stdint.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

#include <hal/log.h>

#define SPI_READ     (0x00)
#define SPI_WRITE    (0x80)

SPIHost::SPIHost(spi_host_device_t host, int mosi_io_num, int miso_io_num, int sclk_io_num, int max_transfer_sz = SPI_MAX_DMA_LEN)
{
    spi_bus_config_t config;

    _handle = host;

    config.mosi_io_num     = mosi_io_num;
    config.miso_io_num     = miso_io_num;
    config.sclk_io_num     = sclk_io_num;
    config.quadwp_io_num   = -1;  // -1 not used
    config.quadhd_io_num   = -1;  // -1 not used
    config.max_transfer_sz = max_transfer_sz;
    config.flags           = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_MOSI;
    config.intr_flags      = 0;

    spi_bus_initialize(host, &config, HSPI_HOST);  // 0 DMA not used
}

spi_device_handle_t SPIHost::attach_device(uint8_t mode, uint32_t clock_speed_hz, int cs_io_num)
{
    spi_device_handle_t             device_handle;
    spi_device_interface_config_t   dev_config;

    dev_config.command_bits     = 0;
    dev_config.address_bits     = 8;
    dev_config.dummy_bits       = 0;
    dev_config.mode             = mode;
    dev_config.duty_cycle_pos   = 128;  // default 128 = 50%/50% duty
    dev_config.cs_ena_pretrans  = 0;    // 0 not used
    dev_config.cs_ena_posttrans = 0;    // 0 not used
    dev_config.clock_speed_hz   = clock_speed_hz;
    dev_config.spics_io_num     = cs_io_num;
    dev_config.flags            = 0;    // 0 not used
    dev_config.queue_size       = 1;
    dev_config.pre_cb           = NULL;
    dev_config.post_cb          = NULL;

    if (spi_bus_add_device(_handle, &dev_config, &device_handle) == ESP_OK)
    {
        return device_handle;
    }
    else
    {
        return NULL;
    }
}

SPIDevice::SPIDevice(SPIHost * host, uint8_t mode, uint32_t clock_speed_hz, int cs_io_num)
{
    _handle = host->attach_device(mode, clock_speed_hz, cs_io_num);
}

esp_err_t SPIDevice::write_byte(uint8_t addr, uint8_t data)
{
    return write_bytes(addr, 1, &data);
}

esp_err_t SPIDevice::write_bytes(uint8_t addr, size_t length, const uint8_t *data)
{
    spi_transaction_t transaction;

    transaction.flags     = 0;
    transaction.cmd       = 0;
    transaction.addr      = addr | SPI_WRITE;
    transaction.length    = length * 8;
    transaction.rxlength  = 0;
    transaction.user      = NULL;
    transaction.tx_buffer = data;
    transaction.rx_buffer = NULL;

    return spi_device_transmit(_handle, &transaction);
}

esp_err_t SPIDevice::read_byte(uint8_t addr, uint8_t *data)
{
    return read_bytes(addr, 1, data);
}

esp_err_t SPIDevice::read_bytes(uint8_t addr, size_t length, uint8_t *data)
{
    spi_transaction_t transaction;

    transaction.flags     = 0;
    transaction.cmd       = 0;
    transaction.addr      = addr | SPI_READ;
    transaction.length    = length * 8;
    transaction.rxlength  = length * 8;
    transaction.user      = NULL;
    transaction.tx_buffer = NULL;
    transaction.rx_buffer = data;
    
    return spi_device_transmit(_handle, &transaction);
}
