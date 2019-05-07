#include <nvs_flash.h>

#include <string.h>

#include <hal/wifi.h>
#include <app/controllers/attitude_controller.h>
#include <hal/udp_server.h>
#include <hal/mdns.h>
#include <utils/matrix.h>
#include <data_model/data_ressources_registry.h>
#include <data_model/json_protocol.h>
#include <os/task.h>

extern "C" void app_main(void)
{
    AttitudeController      * controller;
    Wifi                    * wifi;
    UdpServer               * udp;
    Mdns                    * mdns;
    DataRessourcesRegistry  * registry;
    JsonDataProtocol        * protocol;

    nvs_flash_init();

    wifi       = new Wifi();
    udp        = new UdpServer("quadcopter_control", 5000);
    mdns       = new Mdns("quadcopter", "quadcopter");
    registry   = new DataRessourcesRegistry("data_model.json");
    protocol   = new JsonDataProtocol(udp, registry);
    controller = new AttitudeController(0.02, registry);

    controller->set_height_target(Controller::Mode::SPEED, 0.0);
    controller->set_roll_target(Controller::Mode::POSITION, 0.0);
    controller->set_pitch_target(Controller::Mode::POSITION, 0.0);
    controller->set_yaw_target(Controller::Mode::SPEED, 0.5);

    Task::delay_ms(500);

    wifi->connect();
    mdns->add_service("_quadcopter", "_udp", 5000);
    controller->start();

    while (true)
    {
        Task::delay_ms(1000);
    }
}
