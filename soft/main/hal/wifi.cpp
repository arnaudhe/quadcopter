#include <hal/wifi.h>
#include <esp_wifi.h>
#include <string.h>
#include <esp_err.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <hal/log.h>

#include <hal/wifi_config.h>

esp_err_t Wifi::event_handler(void *ctx, system_event_t *event)
{
    Wifi * wifi = (Wifi *)ctx;

    switch (event->event_id) 
    {
        case SYSTEM_EVENT_STA_START:
            LOG_INFO("SYSTEM_EVENT_STA_START");
            wifi->_state = State::STARTED;
            if (wifi->_connection_request)
            {
                wifi->_connection_request = false;
                ESP_ERROR_CHECK(esp_wifi_connect());
            }
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            wifi->_state = State::CONNECTED;
            LOG_INFO("SYSTEM_EVENT_STA_GOT_IP");
            LOG_INFO("Got IP: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            wifi->_state = State::STARTED;
            LOG_INFO("SYSTEM_EVENT_STA_DISCONNECTED (%d)", event->event_info.disconnected.reason);
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;

        default:
            break;
    }

    return ESP_OK;
}

Wifi::Wifi()
{
    _connection_request = false;
    _disconnection_request = false;

    tcpip_adapter_init();
    esp_event_loop_init(Wifi::event_handler, this);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t sta_config = wifi_config_get();

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
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