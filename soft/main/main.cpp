#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>

#include <string.h>
#include <math.h>

#include <hal/wifi.h>
#include <app/controllers/attitude_controller.h>
#include <hal/udp_server.h>
#include <hal/mdns.h>
#include <utils/matrix.h>
#include <data_model/data_ressources_registry.h>
#include <data_model/json_protocol.h>

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern "C" void app_main(void)
{
    AttitudeController      * controller;
    Wifi                    * wifi;
    UdpServer               * udp;
    Mdns                    * mdns;
    DataRessourcesRegistry  * registry;
    JsonDataProtocol        * protocol;

    nvs_flash_init();

    controller = new AttitudeController(0.01f);
    wifi       = new Wifi();
    udp        = new UdpServer("quadcopter_control", 5000);
    mdns       = new Mdns("quadcopter", "quadcopter");
    registry   = new DataRessourcesRegistry("data_model.json");
    protocol   = new JsonDataProtocol(udp, registry);

    controller->set_height_target(Controller::Mode::SPEED, 0.0);
    controller->set_roll_target(Controller::Mode::POSITION, 0.0);
    controller->set_pitch_target(Controller::Mode::POSITION, 0.0);
    controller->set_yaw_target(Controller::Mode::SPEED, 0.5);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    wifi->connect();
    mdns->add_service("_quadcopter", "_udp", 5000);
    udp->start();

    while (true)
    {
        vTaskDelay(2 / portTICK_PERIOD_MS);
        controller->update();
    }
}
