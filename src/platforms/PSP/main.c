#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include <pspkernel.h>
#include <pspdebug.h>
#include "common/callback.h"
#include <pspctrl.h>
#include <pspdisplay.h>

#define VERS    1 //Talk about this
#define REVS    0

PSP_MODULE_INFO("Gameboy", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER); 
PSP_HEAP_SIZE_MAX();

int main(int argc, char* argv[]) {
    
    int debug = 0;
    char *file_name = 0;
    ClientOrServer cs = NO_CONNECT;
    file_name = argv[1];

    if (!init(file_name, debug, cs)) {
        return 1;
    }
        
    run();
    return 0;
}

