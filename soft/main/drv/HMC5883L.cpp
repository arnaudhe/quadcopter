#include <drv/HMC5883L.h>
#include <drv/HMC5883L_conf.h>
#include <drv/HMC5883L_defs.h>

#include <os/task.h>

#include <hal/log.h>

#include <stdio.h>

HMC5883L::HMC5883L(I2cMaster * i2c)
{
    _i2c     = i2c;
    _address = HMC5883L_ADDRESS;
}

esp_err_t HMC5883L::init(void)
{
    uint8_t   ident_a, ident_b, ident_c;
    uint8_t   count = 0;

    do
    {
        Task::delay_ms(HMC5883L_WAKEUP_DELAY_ms);

        if    ((_i2c->read_register(_address, HMC5883L_REG_IDENT_A, &ident_a) == ESP_OK)
            && (_i2c->read_register(_address, HMC5883L_REG_IDENT_B, &ident_b) == ESP_OK)
            && (_i2c->read_register(_address, HMC5883L_REG_IDENT_C, &ident_c) == ESP_OK))
        {
            if ((ident_a == HMC5885L_IDENT_A) && (ident_b == HMC5885L_IDENT_B) && (ident_c == HMC5885L_IDENT_C))
            {
                LOG_INFO("WHO_I_AM response OK %02x %02x %02x", ident_a, ident_b, ident_c);

                _i2c->write_register(_address, HMC5883L_REG_CONFIG_A, HMC5883L_SAMPLES_8 | HMC5883L_DATARATE_75HZ);
                _i2c->write_register(_address, HMC5883L_REG_CONFIG_B, HMC5883L_MAG_SENSIVITY_SEL);
                _i2c->write_register(_address, HMC5883L_REG_MODE, HMC5883L_CONTINOUS);

                Task::delay_ms(HMC5883L_CONFIG_DELAY_ms);

                return ESP_OK;
            }
            else
            {
                LOG_ERROR("Bad WHO_I_AM response %02x %02x %02x", ident_a, ident_b, ident_c);

                return ESP_FAIL;
            }
        }
        else
        {
            LOG_ERROR("Init failed");
        }
    }while (count++ < 3);

    return ESP_FAIL;
}

esp_err_t HMC5883L::read_mag(float * mag_x, float * mag_y, float * mag_z)
{
    uint8_t buffer[6];
    int16_t temp;

    if (_i2c->read_registers(_address, HMC5883L_REG_OUT_X_M, buffer, 6) == ESP_OK)
    {
        temp = (buffer[0] << 8) + buffer[1];
        *mag_x = ((float)((int)temp) + 200.0) / 9.73;

        temp = (buffer[2] << 8) + buffer[3];
        *mag_z = ((float)((int)temp) + 235.0) / 10.21;

        temp = (buffer[4] << 8) + buffer[5];
        *mag_y = ((float)((int)temp) + 67.0) / 8.94;

        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}