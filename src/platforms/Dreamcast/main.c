
#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../non_core/logger.h"

#include <kos.h>
#include <stdio.h>

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);


int main(int argc, char* argv[]) {
    
    char file_name[1000];
    int debug = 0;
    int dmg_mode = 0;

    ClientOrServer cs = NO_CONNECT;
    set_log_level(LOG_INFO);

    //Initialise 320x240 resolution with 16 bit colors
    vid_set_mode(DM_320x240, PM_RGB565);
    
    //TODO Get File
	
    log_message(LOG_INFO, "loading file %s\n", file_name);

    if (!init_emu(file_name, debug, dmg_mode, cs)) {
        return 1;
    }

    run();
    return 0;
}

