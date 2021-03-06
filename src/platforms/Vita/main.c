#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>

#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include "../../non_core/logger.h"
#include "../../non_core/menu.h"

#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>

#include <stdio.h>


/* TODO HACK:  
 * SDL2_Image has been updated to the point it requires support for webp, which is currently looks like
 * VitakSDK doesn't support. Lets just add dummy symbols ourselves,
 *
 * I'm assuming this will get fixed, and thus will break my hacked fix soon with duplicate webp symbols,
 * so will remove these when that happens or when I find out how to link webp.
 */
void WebPGetFeaturesInternal() {}
void WebPDecodeRGBAInto() {}
void WebPDecodeRGBInto() {}

int _newlib_heap_size_user = 128 * 1024 * 1024;

int main(int argc, char *argv[]) 
{
    scePowerSetArmClockFrequency(444);
   
    // const char *file_name = "ux0:data/Gold.gbc";
    ClientOrServer cs = NO_CONNECT;
    int debug = 0;
    int dmg_mode = 0;

    set_log_level(LOG_INFO);

	//const char *file_name = dir_browse("ux0:");
    char *file_name = NULL;
    int ret_val = 0;
    log_message(LOG_INFO, "Menu\n");
    int result = ui_menu(&file_name, &ret_val);
    if (result != 0 || ret_val != 0)
    {
        return -1;
    }
    else
    {
        printf("ret_val: %d\n", ret_val);
        printf("result: %d\n", result);
    } 

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
