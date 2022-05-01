#pragma once

#include <stdint.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

class SPIHost
{
private:

    spi_host_device_t _handle; /*!< HSPI_HOST or VSPI_HOST */

public:

    SPIHost(spi_host_device_t host, int mosi_io_num, int miso_io_num, int sclk_io_num, int max_transfer_sz);

    spi_device_handle_t attach_device(uint8_t mode, uint32_t clock_speed_hz, int cs_io_num);
};

class SPIDevice
{

private:
    SPIHost * _host;
    spi_device_handle_t _handle;

public:

    SPIDevice(SPIHost * host, uint8_t mode, uint32_t clock_speed_hz, int cs_io_num);

    esp_err_t write_byte(uint8_t addr, uint8_t data);

    esp_err_t write_bytes(uint8_t addr, size_t length, const uint8_t *data);

    esp_err_t read_byte(uint8_t addr, uint8_t *data);

    esp_err_t read_bytes(uint8_t addr, size_t length, uint8_t *data);
};
