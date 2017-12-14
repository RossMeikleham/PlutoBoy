#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>

#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../non_core/logger.h"

#include <stdio.h>

int main(int argc, char *argv[]) 
{
    const char *file_name = "tetris.gb";
    ClientOrServer cs = NO_CONNECT;
    int debug = 0;
    int dmg_mode = 0;

    set_log_level(LOG_INFO);
    clear_log();

    log_message(LOG_INFO, "Starting Emulator\n");
    
     if (!init_emu(file_name, debug, dmg_mode, cs)) {
        log_message(LOG_ERROR, "Failed to initialize emulator\n");
        sceKernelExitProcess(1);
        return 1;
    }
    
    log_message(LOG_INFO, "Running emu\n");
    run();
    sceKernelDelayThread(3*1000000); // Wait for 3 seconds
    sceKernelExitProcess(0);
    return 0;
}
