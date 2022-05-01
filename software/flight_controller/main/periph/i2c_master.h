#pragma once

#include <driver/i2c.h>
#include <os/mutex.h>

using namespace std;

class I2cMaster
{
  
  private: 

    /* Attributes */

    Mutex      _mutex;
    i2c_port_t _port;       ///< i2c port
    bool       _init;

  public:

    /* Constructors */

    I2cMaster(i2c_port_t port);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    esp_err_t deinit();

    esp_err_t read_registers(uint8_t address, uint8_t reg, uint8_t * value, uint8_t len);

    esp_err_t write_registers(uint8_t address, uint8_t reg, uint8_t * value, uint8_t len);

    esp_err_t read_register(uint8_t address, uint8_t reg, uint8_t * value);

    esp_err_t write_register(uint8_t address, uint8_t reg, uint8_t value);

    esp_err_t write_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool value);

    esp_err_t read_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool * value);

};