#pragma once

#include "driver/i2c.h"

using namespace std;

class i2c_master
{
  
  private: 

    /* Attributes */

    i2c_port_t _port;       ///< i2c port
    bool       _init;

    /* Methods */

    esp_err_t _write(uint8_t address, uint8_t * data, uint8_t data_len);

    esp_err_t _read(uint8_t address, uint8_t * data, uint8_t data_len);

  public:

    /* Constructors */

    i2c_master(i2c_port_t port, int frequency_hz);

    /* Accessors*/

    /* Other methods */

    esp_err_t init();

    esp_err_t deinit();

    esp_err_t read_register(uint8_t address, uint8_t reg, uint8_t * value);

    esp_err_t write_register(uint8_t address, uint8_t reg, uint8_t value);

    esp_err_t write_register_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool value);

    esp_err_t read_register_bit(uint8_t address, uint8_t reg, uint8_t bit_num, bool * value);


};