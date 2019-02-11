#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>

#include <string.h>
#include <math.h>

#include <driver/gpio.h>

#include <wifi_credentials.h>
#include <periph/i2c_master.h>
#include <periph/pulse.h>
#include <hal/marg.h>
#include <utils/madgwick_ahrs.h>

#define I2C_MASTER_NUM      I2C_NUM_1         /*!< I2C port number for master dev */

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern "C" void app_main(void)
{
    int             level = 0;
    float           acc_x, acc_y, acc_z;
    float           gyro_x, gyro_y, gyro_z;
    float           mag_x, mag_y, mag_z;
    float           phi, theta, psi;
    i2c_master    * i2c;
    pulse         * puls;
    marg          * mymarg;
    madgwick_ahrs * ahrs;

    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    
    wifi_config_t sta_config;

    strcpy((char *)sta_config.sta.ssid, WIFI_SSID);
    strcpy((char *)sta_config.sta.password, WIFI_PASSWORD);
    sta_config.sta.bssid_set = false;

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    
    i2c    = new i2c_master(I2C_MASTER_NUM);
    mymarg = new marg(i2c);
    ahrs   = new madgwick_ahrs(100.0f);
    puls   = new pulse(2000, 16);

    i2c->init();
    mymarg->init();
    puls->init();

    vTaskDelay(4000 / portTICK_PERIOD_MS);

    puls->set(1000);

    gpio_set_level(GPIO_NUM_4, 1);

    vTaskDelay(4000 / portTICK_PERIOD_MS);

    puls->set(1100);

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    puls->set(1120);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    puls->set(1140);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    puls->set(1160);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    puls->set(1180);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    puls->set(1200);

    while (true)
    {
        gpio_set_level(GPIO_NUM_4, level);

        level = !level;

        vTaskDelay(6 / portTICK_PERIOD_MS);

        mymarg->read_acc(&acc_x, &acc_y, &acc_z);
        mymarg->read_gyro(&gyro_x, &gyro_y, &gyro_z);
        mymarg->read_mag(&mag_x, &mag_y, &mag_z);

        ahrs->update_marg(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);
        ahrs->get_euler(&phi, &theta, &psi);

        printf("%f;%f;%f\n", phi, theta, psi);
    }
}

