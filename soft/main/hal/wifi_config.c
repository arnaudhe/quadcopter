#include "hal/wifi_config.h"

#define EXAMPLE_ESP_WIFI_SSID      "AirPort-Opera"
#define EXAMPLE_ESP_WIFI_PASS      "icicestlopera"

static wifi_config_t config = {
    .sta = {
        .ssid = EXAMPLE_ESP_WIFI_SSID,
        .password = EXAMPLE_ESP_WIFI_PASS
    },
};

wifi_config_t wifi_config_get(void)
{
    return config;
}