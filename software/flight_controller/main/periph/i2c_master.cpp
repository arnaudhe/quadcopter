#include <periph/i2c_master.h>
#include <periph/i2c_master_defs.h>
#include <periph/i2c_master_conf.h>
#include <esp_attr.h>

I2cMaster::I2cMaster(i2c_port_t port):
    _mutex()
{
    _port = port;
    _init = false;
}

esp_err_t I2cMaster::init()
{
    esp_err_t    ret;
    i2c_config_t conf;

    if (_init == false)
    {
        conf.mode             = I2C_MODE_MASTER;
        conf.sda_io_num       = I2C_MASTER_CONF[_port].mapping.sda_io_num;
        conf.sda_pullup_en    = GPIO_PULLUP_ENABLE;
        conf.scl_io_num       = I2C_MASTER_CONF[_port].mapping.scl_io_num;
        conf.scl_pullup_en    = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = I2C_MASTER_CONF[_port].frequency;

        if ((ret = i2c_param_config(_port, &conf)) != ESP_OK) return ret;

        if ((ret = i2c_driver_install(_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0)) != ESP_OK) return ret;

        _init = true;
    }

    return ESP_OK;
}

esp_err_t I2cMaster::deinit()
{
    return ESP_OK;
}

esp_err_t IRAM_ATTR I2cMaster::read_registers(uint8_t address, uint8_t reg, uint8_t * value, uint8_t len)
{
    esp_err_t        ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, value, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    _mutex.lock();
    ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
    _mutex.unlock();
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t IRAM_ATTR I2cMaster::write_registers(uint8_t address, uint8_t reg, uint8_t * value, uint8_t len)
{
    esp_err_t        ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write(cmd, value, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    _mutex.lock();
    ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
    _mutex.unlock();
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t IRAM_ATTR I2cMaster::read_register(uint8_t address, uint8_t reg, uint8_t * value)
{
    return read_registers(address, reg, value, 1);
}

esp_err_t IRAM_ATTR I2cMaster::write_register(uint8_t address, uint8_t reg, uint8_t value)
{
    return write_registers(address, reg, &value, 1);
}

esp_err_t IRAM_ATTR I2cMaster::write_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool value)
{
    uint8_t   b;
    esp_err_t ret;

    ret = read_register(address, reg, &b);

    if (ret == ESP_OK)
    {
        b = (value != 0) ? (b | (1 << bit_num)) : (b & ~(1 << bit_num));
        return write_register(address, reg, b);
    }
    else
    {
        return ret;
    }
}

esp_err_t IRAM_ATTR I2cMaster::read_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool * value)
{
    uint8_t   b;
    esp_err_t ret;

    ret = read_register(address, reg, &b);

    if (ret == ESP_OK)
    {
        *value = (bool)((b >> bit_num) & 0x01);
    }

    return ret;
}
