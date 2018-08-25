#include "../../non_core/logger.h"
#include <stdarg.h>
#include <stdio.h>

static LogLevel current_log_level = LOG_OFF;

void set_log_level(LogLevel ll) {

    if (ll >= LOG_OFF) {
        current_log_level = LOG_OFF;
    } else {
        current_log_level = ll;
    }
}

void log_message(LogLevel ll, const char *fmt, ...) {
   
    if(ll <= LOG_OFF && ll >= current_log_level) {
        // Choose output stream based on current log level
        FILE *stream = (ll == LOG_INFO) ? stdout : stderr;

        // Log message level
        char *level_str = "";
        switch (ll) {
            case LOG_INFO : 
                level_str = "INFO"; 
                fprintf(stream,"[\x1b[32m%s\x1b[0m]: ",level_str);
                break;
            case LOG_WARN : 
                level_str = "WARN"; 
                fprintf(stream,"[\x1b[33m%s\x1b[0m]: ",level_str);
                break;
            case LOG_ERROR: 
                level_str = "ERROR"; 
                fprintf(stream,"[\x1b[31m%s\x1b[0m]: ",level_str);
                break;
            default : 
                level_str = "UNKNOWN"; // Shouldn't happen 
                fprintf(stream,"[\x1b[31m%s\x1b[0m]: ",level_str);
        }
        
        // Log message
        va_list args;
        va_start(args, fmt);
        vfprintf(stream,fmt,args); 
        va_end(args); 
    } 
}


