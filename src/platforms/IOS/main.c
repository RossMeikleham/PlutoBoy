
// IOS main file

#include <SDL2/SDL.h>
#include "../../../src/core/emu.h"
#include "../../../src/non_core/menu.h"
#include "../../../src/non_core/logger.h"

//#import <Foundation/Foundation.h>


int main(int argc, char *argv[]) {
    
    int res;
    set_log_level(LOG_INFO);
    
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
    
    
    res =init_emu("rom.gb", 0, 0, NO_CONNECT);
    run();

    return 0;
}
