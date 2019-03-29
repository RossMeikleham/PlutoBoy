#include <string.h>
#include <stdio.h>
#include <switch.h>
#include <SDL2/SDL.h>

#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../core/mmu/mbc.h"
#include "../../non_core/logger.h"

void cleanup() {
    
    finalize_emu();
    SDL_Quit();
}

int main(int argc, char **argv) {

    int debug = 0;
    int cgb_mode = 1;
    char file_name[1024] = "sdmc:/rom.gb";

    ClientOrServer cs = NO_CONNECT;
   
    printf("TEST HELLO WORLD\n"); 
  
    set_log_level(LOG_INFO);

    if (!init_emu(file_name, debug, !cgb_mode, cs)) {
        log_message(LOG_ERROR, "Failed to initialize emulator\n");
        cleanup();
        return 1;
    }
    
    log_message(LOG_INFO, "Running emu\n");
    run();

    write_SRAM();
    cleanup();
	return 0;
}
