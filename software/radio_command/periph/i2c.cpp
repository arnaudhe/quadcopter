
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <alloca.h>

#include "i2c.hpp"

I2C::I2C(const char * port)
{
    this->fd = ::open(port, O_RDWR);

    if (this->fd < 0)
    {
        fprintf(stderr, "Failed to open the i2c bus : %s\n", strerror(errno));
        exit(1);
    }

    this->slave_addr = 0x00;
}

bool I2C::_set_slave_addr(uint8_t slave_addr)
{
    if (this->slave_addr == slave_addr)
    {
        return true;
    }

    if (ioctl(this->fd, I2C_SLAVE, slave_addr) < 0)
    {
        fprintf(stderr, "Failed to set slave addr : %s\n", strerror(errno));
        return false;
    }

    this->slave_addr = slave_addr;
    return true;
}

bool I2C::_read_bus(uint8_t * data, size_t len)
{
    if (read(this->fd, data, len) < 0)
    {
        fprintf(stderr, "Failed to read on bus : %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool I2C::_write_bus(uint8_t * data, size_t len)
{
    if (write(this->fd, data, len) < 0)
    {
        fprintf(stderr, "Failed to write on bus : %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool I2C::read_registers(uint8_t slave_addr, uint8_t reg_addr, size_t len, uint8_t * value)
{
    if (_set_slave_addr(slave_addr) == false)
    {
        return false;
    }

    if (_write_bus(&reg_addr, 1) == false)
    {
        return false;
    }

    if (_read_bus(value, len) == false)
    {
        return false;
    }

    return true;
}

bool I2C::write_registers(uint8_t slave_addr, uint8_t reg_addr, size_t len, uint8_t * value)
{
    uint8_t * buffer = (uint8_t *)alloca(len + 1);

    if (_set_slave_addr(slave_addr) == false)
    {
        return false;
    }

    buffer[0] = reg_addr;
    memcpy(&buffer[1], value, len);

    if (_write_bus(buffer, len + 1) == false)
    {
        return false;
    }

    return true;
}

bool I2C::read_register(uint8_t slave_addr, uint8_t reg_addr, uint8_t * value)
{
    return read_registers(slave_addr, reg_addr, 1, value);
}

bool I2C::write_register(uint8_t slave_addr, uint8_t reg_addr, uint8_t value)
{
    return write_registers(slave_addr, reg_addr, 1, &value);
}

bool I2C::close()
{
    ::close(this->fd);

    return true;
}
