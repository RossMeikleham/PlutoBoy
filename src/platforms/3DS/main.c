#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include "SDL/SDL.h"

#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../non_core/logger.h"
#include "debug_info.h"

#include "3DES/3des.h"

//u32 __stacksize__ = 64 * (1024 * 1024); // 20MB stack size

int64_t higherClkRate = 0;
int64_t L2CacheEnabled = 0;
int64_t clkRate = 0;

void cleanup() {
    
    finalize_emu();
    gfxFlushBuffers();
	gfxSwapBuffers();
	gspWaitForVBlank();
    gfxExit();
    mcuHwcExit();
}

int main(int argc, char **argv) {

    mcuHwcInit();
    gfxInitDefault();
    
    int debug = 0;
    int cgb_mode = 0;
    char file_name[1024];
    if (!selectFile(&cgb_mode, file_name)) {
        cleanup();
        return 0;
    };

    gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);   
    
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, &debug_screen);
    consoleInit(GFX_BOTTOM, &fps_screen);
    consoleInit(GFX_BOTTOM, &battery_screen);
    consoleInit(GFX_BOTTOM, &fps_limiter_screen);

    //x, y, width, height
    consoleSetWindow(&fps_screen, 0, 0, 11, 2);
    consoleSetWindow(&fps_limiter_screen, 11, 0, 17, 2);
    consoleSetWindow(&battery_screen, 28, 0, 13, 2);
    consoleSetWindow(&debug_screen, 0, 3, 40, 27);

    ClientOrServer cs = NO_CONNECT;
    
    set_log_level(LOG_INFO);

    osSetSpeedupEnable(1);

    if (!init_emu(file_name, debug, !cgb_mode, cs)) {
        log_message(LOG_ERROR, "Failed to initialize emulator\n");
        cleanup();
        return 1;
    }
    
    log_message(LOG_INFO, "Running emu\n");
	run();


    cleanup();
	return 0;
}
