#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>


int main() {
    
    int debug = 0;
    char *file_name = "rom.gb";
    int dmg = 0;

    ClientOrServer cs = NO_CONNECT;

    if (!init_emu(file_name, debug, dmg, cs)) {
        return 1;
    }

    // Run main loop at 60fps
    emscripten_set_main_loop(run_one_frame, 60, 1);
        
    return 0;
}


