#include "../../non_core/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <android/log.h>

#define TAG "PlutoBoy"   

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
      
        // Log message level
        unsigned droid_log_level;
        switch (ll) {
            case LOG_INFO: 
                droid_log_level = ANDROID_LOG_INFO;
                break;
            case LOG_WARN: 
                droid_log_level = ANDROID_LOG_WARN;
                break;
            case LOG_ERROR: 
                droid_log_level = ANDROID_LOG_ERROR;
                break;
            default:
                return;
        }
        
        va_list args;
        va_start(args, fmt);
        __android_log_vprint(droid_log_level, TAG, fmt, args); 
        va_end(args);        
    } 
}


