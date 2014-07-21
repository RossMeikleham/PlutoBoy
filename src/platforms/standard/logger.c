/*
 * =====================================================================================
 *
 *       Filename:  logger.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/14 14:31:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "../../non_core/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>


static LogLevel current_log_level = LOG_OFF;;

void set_log_level(LogLevel ll) {
    
    if (ll >= LOG_OFF) {
        current_log_level = LOG_OFF;
    } else {
        current_log_level = ll;
    }
}

//Logs the current time to the given stream
static void log_time(FILE *stream) {
    
    char buffer[20];
    struct tm *str_time;

    time_t now = time (0);
    str_time = gmtime(&now);

    strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S ", str_time);
    fprintf(stream,"%s ",buffer);
}



void log_message(LogLevel ll, const char *fmt, ...) {
    
    if(ll <= LOG_OFF && ll >= current_log_level) {
        // Choose output stream based on current log level
        FILE *stream = (ll == LOG_INFO) ? stdout : stderr;

        log_time(stream);
        // Log message level
        char *level_str;
        switch (ll) {
            case LOG_INFO : level_str = "INFO"; break;
            case LOG_WARN : level_str = "WARN"; break;
            case LOG_ERROR: level_str = "ERROR"; break;
            default : level_str = "UNKNOWN"; // Shouldn't happen 
        }
        fprintf(stream,"[%s]: ",level_str);

        // Log message
        va_list args;
        va_start(args, fmt);
        vfprintf(stream,fmt,args); 
        va_end(args);        
    } 
}


