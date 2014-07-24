#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C"{
#endif

// Different Logging levels
typedef enum {LOG_INFO, LOG_WARN, LOG_ERROR, LOG_OFF} LogLevel;

// Set one of the 4 specified logging levels
void set_log_level(LogLevel ll);

/* Log a given message with the specified logging level,
 * if the current logging level set it greater than or equal
 * to the level supplied (and neither are OFF) then the message
 * will be successfully logged. */
void log_message(LogLevel ll, const char *fmt, ...); 

#ifdef __cplusplus
}
#endif

#endif //LOGGER_H
