#include "file_browser/browse.h"

#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../non_core/logger.h"


#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>

#define VERS    1 
#define REVS    0

//PSP_MODULE_INFO("Gameboy", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER); 
PSP_HEAP_SIZE_KB(7000);
PSP_MAIN_THREAD_STACK_SIZE_KB(7000);

int main(int argc, char* argv[]) {
    
    char file_name[1000];
    int debug = 0;
    int dmg_mode = 0;

    ClientOrServer cs = NO_CONNECT;
    set_log_level(LOG_INFO);
    
    if ((dmg_mode = get_gb_file(file_name)) < 0) {
        log_message(LOG_ERROR, "failed to load file\n");
        return 1;
    }
	
    log_message(LOG_INFO, "loading file %s\n", file_name);

    if (!init_emu(file_name, debug, dmg_mode, cs)) {
        return 1;
    }

    run();
    return 0;
}

