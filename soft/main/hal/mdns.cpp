#include <mdns.h>
#include <hal/mdns.h>
#include <esp_log.h>

Mdns::Mdns(string hostname, string instance_name)
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) 
    {
        ESP_LOGE("MDNS", "MDNS Init failed: %d\n", err);
        return;
    }

    ESP_ERROR_CHECK(mdns_hostname_set(hostname.c_str()));
    ESP_ERROR_CHECK(mdns_instance_name_set(instance_name.c_str()));
}

void Mdns::add_service(string name, string protocol, int port)
{
    ESP_ERROR_CHECK(mdns_service_add("Quadcopter-Control-Interface", name.c_str(), protocol.c_str(), port, NULL, 0));
}