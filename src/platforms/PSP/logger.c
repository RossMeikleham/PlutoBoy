#include "../../non_core/logger.h"
#include <stdarg.h>
#include <stdio.h>

static LogLevel current_log_level = LOG_OFF;

static const char *logfile = "plutoboy.log";

void clear_log() {
   FILE *f = fopen(logfile, "w"); 
   fprintf(f, "Plutoboy Logfile\n----------------\n");
   fclose(f);
}

void set_log_level(LogLevel ll) {
    
	if (current_log_level == LOG_OFF) {
		clear_log();
	}

    if (ll >= LOG_OFF) {
        current_log_level = LOG_OFF;
    } else {
        current_log_level = ll;
    }
}

void log_message(LogLevel ll, const char *fmt, ...) {
   
    if(ll <= LOG_OFF && ll >= current_log_level) {
        // Choose output stream based on current log level
        //FILE *stream = (ll == LOG_INFO) ? stdout : stderr;
        FILE *stream = fopen(logfile, "a");

        // Log message level
        char *level_str = "";
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
        
        fclose(stream);      
    } 
}

/*


static LogLevel current_log_level = LOG_OFF;;

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
*/
