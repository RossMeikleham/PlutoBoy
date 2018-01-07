
// IOS main file

#include <SDL2/SDL.h>
#include "../../../src/core/emu.h"


int main(int argc, char *argv[]) {
    
    int res;
    res =init_emu("rom.gb", 0, 0, NO_CONNECT);
    run();

    return 0;
}
