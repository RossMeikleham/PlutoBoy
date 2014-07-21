#ifndef LOGGER_H
#define LOGGER_H

typedef enum {LOG_INFO, LOG_WARN, LOG_ERROR, LOG_OFF} LogLevel;

void set_log_level(LogLevel ll);

void log_message(LogLevel ll, const char *fmt, ...); 

#endif
