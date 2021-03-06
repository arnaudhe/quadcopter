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


    /* Methods */

    esp_err_t _write(uint8_t address, uint8_t * data, uint8_t data_len);

    esp_err_t _read(uint8_t address, uint8_t * data, uint8_t data_len);

  public:

    /* Constructors */

    I2cMaster(i2c_port_t port);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    esp_err_t deinit();

    esp_err_t read(uint8_t address, uint8_t reg, uint8_t * value, uint8_t len);

    esp_err_t read_register(uint8_t address, uint8_t reg, uint8_t * value);

    esp_err_t write_register(uint8_t address, uint8_t reg, uint8_t value);

    esp_err_t write_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool value);

    esp_err_t read_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool * value);

    esp_err_t read_int(uint8_t address, uint8_t reg, int16_t * value);

    esp_err_t read_uint(uint8_t address, uint8_t reg, uint16_t * value);

};