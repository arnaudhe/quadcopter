#pragma once

#include <hal/udp_server.h>
#include <stdarg.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_LEVEL_VERBOSE       0       // Module's developer useful data. Can be very verbose. (buffer dumps, bus data dump, ...)
#define LOG_LEVEL_DEBUG         1       // Debugging data that can help understanding the execution (state machine transitions, variable value, ...)
#define LOG_LEVEL_INFO          2       // A general event (not periodic) about runtime (connection done, system disarmed, alarm triggered, ...)
#define LOG_LEVEL_WARNING       3       // An external (not caused by runtime) defective event that MAY affect the runtime (frame lost, data corrupted, ...)
#define LOG_LEVEL_ERROR         4       // A defect that WILL affect the runtime (communication failure, bad state, ...)
#define LOG_LEVEL_DISABLE       0xff    // Use to disable the logs

#define LOG_LEVEL               LOG_LEVEL_DEBUG

#define LOG_VERBOSE_FILE        "file.cpp"

#if (LOG_LEVEL <= LOG_LEVEL_VERBOSE)
#define LOG_VERBOSE(fmt, ...)   Logger::Verbose(__FILENAME__, fmt, ##__VA_ARGS__)
#else
#define LOG_VERBOSE(fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(fmt, ...)     Logger::Debug(__FILENAME__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define LOG_INFO(fmt, ...)      Logger::Info(__FILENAME__, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
#define LOG_WARNING(fmt, ...)   Logger::Warning(__FILENAME__, fmt, ##__VA_ARGS__)
#else
#define LOG_WARNING(fmt, ...)
#endif

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define LOG_ERROR(fmt, ...)     Logger::Error(__FILENAME__, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

class Logger
{
  private:

    UdpServer * _udp;

  public:

    Logger(UdpServer * udp);

    static void Verbose(const char * filename, const char * fmt, ...);
    static void Debug(const char * filename, const char * fmt, ...);
    static void Info(const char *filename, const char *fmt, ...);
    static void Warning(const char *filename, const char *fmt, ...);
    static void Error(const char *filename, const char *fmt, ...);
};