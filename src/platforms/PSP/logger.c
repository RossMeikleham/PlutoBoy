#include "../../non_core/logger.h"


static LogLevel current_log_level = LOG_OFF;;

void set_log_level(LogLevel ll) {
    
    if (ll >= LOG_OFF) {
        current_log_level = LOG_OFF;
    } else {
        current_log_level = ll;
    }
}



void log_message(LogLevel ll, const char *fmt, ...) {
    
}


