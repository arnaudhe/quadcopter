#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include <math.h>

#include "driver/gpio.h"

#include "wifi_credentials.h"
#include "MadgwickAHRS.h"
#include "hal/mpu6050_defs.h"
#include "hal/hmc5883L_defs.h"


#define DATA_LENGTH                   512               /*!< Data buffer length for test buffer*/
#define RW_TEST_LENGTH                129               /*!< Data length for r/w test, any value from 0-DATA_LENGTH*/
#define DELAY_TIME_BETWEEN_ITEMS_MS   1234              /*!<  delay time between different test items */

#define I2C_MASTER_SCL_IO             19                /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO             18                /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM                I2C_NUM_1         /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE     0                 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE     0                 /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ            200000            /*!< I2C master clock frequency */

#define WRITE_BIT                     I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT                      I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN                  0x1               /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                 0x0               /*!< I2C master will not check ack from slave */
#define ACK_VAL                       0x0               /*!< I2C ack value */
#define NACK_VAL                      0x1               /*!< I2C nack value */


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

esp_err_t _write(i2c_port_t i2c_num, uint8_t address, uint8_t * data, uint8_t data_len)
{
    esp_err_t        ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t _read(i2c_port_t i2c_num, uint8_t address, uint8_t * data, uint8_t data_len)
{
    esp_err_t        ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, data_len, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t _read_register(i2c_port_t i2c_num, uint8_t address, uint8_t reg, uint8_t * value)
{
    esp_err_t ret;

    ret = _write(i2c_num, address, &reg, 1);

    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = _read(i2c_num, address, value, 1);

    return ret;
}

esp_err_t _write_register(i2c_port_t i2c_num, uint8_t address, uint8_t reg, uint8_t value)
{
    esp_err_t ret;
    uint8_t buffer[2] = {reg, value};

    return _write(i2c_num, address, buffer, 2);
}

esp_err_t _write_bit(i2c_port_t i2c_num, uint8_t address, uint8_t reg, uint8_t bit_num, bool value)
{
    uint8_t   b;
    esp_err_t ret;
    
    ret = _read_register(i2c_num, address, reg, &b);

    if (ret == ESP_OK)
    {
        b = (value != 0) ? (b | (1 << bit_num)) : (b & ~(1 << bit_num));
        return _write_register(i2c_num, address, reg, b);
    }
    else
    {
        return ret;
    }
}

esp_err_t _read_bit(i2c_port_t i2c_num, uint8_t address, uint8_t reg, uint8_t bit_num, bool * value)
{
    uint8_t   b;
    esp_err_t ret;
    
    ret = _read_register(i2c_num, address, reg, &b);

    if (ret == ESP_OK)
    {
        *value = (bool)((b >> bit_num) & 0x01);
    }
    
    return ret;
}

/**
 * @brief i2c master initialization
 */
void i2c_master_init()
{
    int           i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t  conf;

    conf.mode             = I2C_MODE_MASTER;
    conf.sda_io_num       = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en    = GPIO_PULLUP_ENABLE;
    conf.scl_io_num       = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en    = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief test function to show buffer
 */
void disp_buf(uint8_t* buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
        if (( i + 1 ) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void i2c_test(void)
{
    esp_err_t ret;
    uint8_t   who_i_am;
    
    ret = _read_register( I2C_MASTER_NUM, MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, &who_i_am);
    
    printf("********************\n");
    printf(" MASTER READ SENSOR \n");
    printf("********************\n");

    if (ret == ESP_OK)
    {
        printf("data: %02x\n", who_i_am);
    }
    else 
    {
        printf("No ack, sensor not connected...skip...\n");
    }
}

void app_main(void)
{
    int level = 0;
    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z;
    float mag_x, mag_y, mag_z;
    float phi, theta, psi;

    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    
    wifi_config_t sta_config = {
        .sta = {
            .ssid      = WIFI_SSID,
            .password  = WIFI_PASSWORD,
            .bssid_set = false
        }
    };

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    
    i2c_master_init();
    _mpu6050_init();
    _hmc5883_init();

    while (true)
    {
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;

        vTaskDelay(6 / portTICK_PERIOD_MS);

        _mpu6050_read_acc(&acc_x, &acc_y, &acc_z);
        _mpu6050_read_gyro(&gyro_x, &gyro_y, &gyro_z);
        _hmc5883_read_mag(&mag_x, &mag_y, &mag_z);

        MadgwickAHRSupdate(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);
        MadgwickAHRSGetEuler(&phi, &theta, &psi);

        //printf("%f\t%f\t%f\t", acc_x,  acc_y,  acc_z);
        //printf("%f\t%f\t%f\n", gyro_x, gyro_y, gyro_z);
        printf("%f\t%f\t%f\n", phi, theta, psi);
        //printf("%f\n", atan2(mag_x, mag_y) * 180.0 / 3.1416);

    }
}

