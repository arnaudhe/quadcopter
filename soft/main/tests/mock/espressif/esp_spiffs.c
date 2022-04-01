#include <esp_spiffs.h>

const char * DATA_MODEL_PATH = SOURCE_DIR "/../../../data_model";

esp_err_t esp_vfs_spiffs_register(esp_vfs_spiffs_conf_t * conf)
{
    conf->base_path = DATA_MODEL_PATH;
    return ESP_OK;
}

esp_err_t esp_spiffs_info(const char* partition_label, size_t *total_bytes, size_t *used_bytes)
{
    return ESP_OK;
}

esp_err_t esp_vfs_spiffs_unregister(const char* partition_label)
{
    return ESP_OK;
}
