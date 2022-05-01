#pragma once

#include <stdio.h>

#define ESP_LOGE(domain, format, ...)   { printf("ERR %s: ", domain); printf(format, ##__VA_ARGS__); printf("\n"); }
#define ESP_LOGW(domain, format, ...)   { printf("WRN %s: ", domain); printf(format, ##__VA_ARGS__); printf("\n"); }
#define ESP_LOGI(domain, format, ...)   { printf("INF %s: ", domain); printf(format, ##__VA_ARGS__); printf("\n"); }
#define ESP_LOGD(domain, format, ...)   { printf("DBG %s: ", domain); printf(format, ##__VA_ARGS__); printf("\n"); }