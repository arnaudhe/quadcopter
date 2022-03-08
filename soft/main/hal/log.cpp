#include <esp_log.h>
#include <hal/log.h>

#define LOGGER_DEST_PORT    5004

Logger * _logger = NULL;

Logger::Logger(UdpServer *udp)
{
    _udp = udp;
    _logger = this;
}

void IRAM_ATTR Logger::Verbose(const char * filename, const char * fmt, ...)
{
    char buffer[256];
    va_list args;

    if ((strcmp(filename, LOG_VERBOSE_FILE) == 0) ||
        (strcmp("*", LOG_VERBOSE_FILE) == 0))
    {
        va_start(args, fmt);
        vsprintf(buffer, fmt, args);
        va_end(args);
        esp_log_write(ESP_LOG_VERBOSE, filename, LOG_COLOR_V "V (%d) %s: %s\n" LOG_RESET_COLOR, esp_log_timestamp(), filename, buffer);

        if (_logger)
        {
            _logger->_udp->send_broadcast(buffer, LOGGER_DEST_PORT);
        }
    }
}

void IRAM_ATTR Logger::Debug(const char * filename, const char * fmt, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    esp_log_write(ESP_LOG_DEBUG, filename, LOG_COLOR_D "D (%d) %s: %s\n" LOG_RESET_COLOR, esp_log_timestamp(), filename, buffer);

    if (_logger)
    {
        _logger->_udp->send_broadcast(buffer, LOGGER_DEST_PORT);
    }
}

void IRAM_ATTR Logger::Info(const char * filename, const char * fmt, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    esp_log_write(ESP_LOG_INFO, filename, LOG_COLOR_I "I (%d) %s: %s\n" LOG_RESET_COLOR, esp_log_timestamp(), filename, buffer);

    if (_logger)
    {
        _logger->_udp->send_broadcast(buffer, LOGGER_DEST_PORT);
    }
}

void IRAM_ATTR Logger::Warning(const char * filename, const char * fmt, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    esp_log_write(ESP_LOG_WARN, filename, LOG_COLOR_W "W (%d) %s: %s\n" LOG_RESET_COLOR, esp_log_timestamp(), filename, buffer);

    if (_logger)
    {
        _logger->_udp->send_broadcast(buffer, LOGGER_DEST_PORT);
    }
}

void IRAM_ATTR Logger::Error(const char * filename, const char * fmt, ...)
{
    char buffer[256];
    va_list args;

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    esp_log_write(ESP_LOG_ERROR, filename, LOG_COLOR_E "E (%d) %s: %s\n" LOG_RESET_COLOR, esp_log_timestamp(), filename, buffer);

    if (_logger)
    {
        _logger->_udp->send_broadcast(buffer, LOGGER_DEST_PORT);
    }
}