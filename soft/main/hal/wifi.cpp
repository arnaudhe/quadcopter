#include <esp_netif.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <hal/log.h>
#include <hal/wifi.h>
#include <hal/wifi_config.h>

#include <string.h>

void Wifi::event_handler(void *arg, esp_event_base_t event_base,
                         int32_t event_id, void *event_data)
{
    Wifi *wifi = (Wifi *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
      LOG_INFO("Connect to the AP");
      esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        esp_wifi_connect();
        LOG_INFO("retry to connect to the AP");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        LOG_INFO("Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

Wifi::Wifi()
{
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    _connection_request = false;
    _disconnection_request = false;

    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, this, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, this, &instance_got_ip));

    wifi_config_t sta_config = wifi_config_get();

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void Wifi::connect(void)
{
    if (_state == State::STARTED)
    {
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if (_state == State::OFF)
    {
        _connection_request = true;
    }
}