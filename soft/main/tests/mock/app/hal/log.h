#pragma once

#include <stdio.h>

#define LOG_ERROR(format, ...)   { printf("<ERR> "); printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_WARNING(format, ...) { printf("<WRN> "); printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_INFO(format, ...)    { printf("<INF> "); printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_DEBUG(format, ...)   { printf("<DBG> "); printf(format, ##__VA_ARGS__); printf("\n"); }
#define LOG_VERBOSE(format, ...) { printf("<VER> "); printf(format, ##__VA_ARGS__); printf("\n"); }
