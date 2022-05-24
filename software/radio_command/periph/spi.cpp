#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <string.h>

#include "spi.hpp"

bool Spi::_transfer(uint8_t * tx, uint8_t * rx, size_t len)
{
    struct spi_ioc_transfer transfer;

    memset(&transfer, 0, sizeof(transfer));

    transfer.tx_buf        = (unsigned long)tx;
    transfer.rx_buf        = (unsigned long)rx;
    transfer.len           = len;
    transfer.speed_hz      = this->speed;
    transfer.delay_usecs   = this->delay;
    transfer.bits_per_word = this->bits;
#ifdef SPI_IOC_WR_MODE32
    transfer.tx_nbits = 0;
#endif
#ifdef SPI_IOC_RD_MODE32
    transfer.rx_nbits = 0;
#endif

    if (ioctl(this->fd, SPI_IOC_MESSAGE(1), &transfer) < 1)
    {
        fprintf(stderr, "Failed to transfer spi message\n");
        return false;
    }

    return true;
}

bool Spi::open(const char * port, Mode mode, uint32_t speed_hz)
{
    this->mode  = mode;
    this->speed = speed_hz;
    this->bits  = 8;
    this->delay = 3;

    this->fd = ::open(port, O_RDWR);
    if (this->fd < 0)
    {
        fprintf(stderr, "Failed to open the spi bus : %s\n", strerror(errno));
        return false;
    }

    /* spi mode */

    if (ioctl(this->fd, SPI_IOC_WR_MODE, &this->mode) < 0)
    {
        fprintf(stderr, "can't set spi mode\n");
        return false;
    }

    if (ioctl(this->fd, SPI_IOC_RD_MODE, &this->mode) < 0)
    {
        fprintf(stderr, "can't get spi mode\n");
        return false;
    }

    /* bits per word */

    if (ioctl(this->fd, SPI_IOC_WR_BITS_PER_WORD, &this->bits) < 0)
    {
        fprintf(stderr, "can't set bits per word\n");
        return false;
    }

    if (ioctl(this->fd, SPI_IOC_RD_BITS_PER_WORD, &this->bits) < 0)
    {
        fprintf(stderr, "can't get bits per word\n");
        return false;
    }

    /* max speed hz */

    if (ioctl(this->fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed) < 0)
    {
        fprintf(stderr, "can't set max speed hz\n");
        return false;
    }

    if (ioctl(this->fd, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed) < 0)
    {
        fprintf(stderr, "can't get max speed hz\n");
        return false;
    }

    printf("speed %d\n", this->speed);

    return true;
}

bool Spi::read_register(uint8_t reg_addr, uint8_t * value)
{
    return read_registers(reg_addr, 1, value);
}

bool Spi::write_register(uint8_t reg_addr, uint8_t value)
{
    return write_registers(reg_addr, 1, &value);
}

bool Spi::read_registers(uint8_t reg_addr, size_t len, uint8_t * value)
{
    uint8_t * tx = (uint8_t *)alloca(len + 1);
    uint8_t * rx = (uint8_t *)alloca(len + 1);

    memset(tx, 0, len);
    memset(rx, 0, len);

    tx[0] = reg_addr;

    if (_transfer(tx, rx, len + 1) == false)
    {
        return false;
    }

    *value = rx[1];

    return true;
}

bool Spi::write_registers(uint8_t reg_addr, size_t len, uint8_t * value)
{
    uint8_t * tx = (uint8_t *)alloca(len + 1);
    uint8_t * rx = (uint8_t *)alloca(len + 1);

    memset(tx, 0, len);
    memset(rx, 0, len);

    tx[0] = reg_addr | 0x80;
    memcpy(&tx[1], value, len);

    if (_transfer(tx, rx, len + 1) == false)
    {
        return false;
    }

    return true;
}


bool Spi::close(void)
{
    ::close(this->fd);
    return true;
}
