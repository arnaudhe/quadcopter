#pragma once

#include <esp_log.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(fmt, ...)     ESP_LOGD(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)      ESP_LOGI(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)   ESP_LOGW(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     ESP_LOGE(__FILENAME__, fmt, ##__VA_ARGS__)
