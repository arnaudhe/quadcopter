#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <esp_err.h>

typedef struct {
    char* base_path;          /*!< File path prefix associated with the filesystem. */
    char* partition_label;    /*!< Optional, label of SPIFFS partition to use. If set to NULL, first partition with subtype=spiffs will be used. */
    size_t max_files;               /*!< Maximum files that could be open at the same time. */
    bool format_if_mount_failed;    /*!< If true, it will format the file system if it fails to mount. */
} esp_vfs_spiffs_conf_t;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_vfs_spiffs_register(esp_vfs_spiffs_conf_t * conf);

esp_err_t esp_spiffs_info(const char* partition_label, size_t *total_bytes, size_t *used_bytes);

esp_err_t esp_vfs_spiffs_unregister(const char* partition_label);

#ifdef __cplusplus
}
#endif