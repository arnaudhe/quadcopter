#pragma once

#include <hal/udp_server.h>
#include <stdarg.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(fmt, ...)     Logger::Debug(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)      Logger::Info(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)   Logger::Warning(__FILENAME__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     Logger::Error(__FILENAME__, fmt, ##__VA_ARGS__)

class Logger
{
  private:

    UdpServer * _udp;

  public:

    Logger(UdpServer * udp);

    static void Debug(const char * filename, const char * fmt, ...);
    static void Info(const char *filename, const char *fmt, ...);
    static void Warning(const char *filename, const char *fmt, ...);
    static void Error(const char *filename, const char *fmt, ...);
};