
// Android main file

#include "SDL.h"

#include "../../../src/core/emu.h"
#include "../../../src/non_core/logger.h"
#include <android/log.h>

int main(int argc, char *argv[]) {

    
    int res;
    set_log_level(LOG_INFO);
    log_message(LOG_INFO, "Initing Emu");
    res = init_emu("rom.gb", 0, 0, NO_CONNECT);
    
    if (res) {
        run();
    }

    return 0;
}
