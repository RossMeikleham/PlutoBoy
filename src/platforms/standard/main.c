#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *prog_name;

#define ARG_ERR {printf("usage %s [options] rom_file\n", prog_name);\
                printf("type %s -help for detailed help\n", prog_name);\
                exit(0);}

ClientOrServer getConnection(char *arg) {
    if (!strcmp(arg, "client")) {return CLIENT;}
    else if(!strcmp(arg, "server")) {return SERVER;}
    else ARG_ERR;
}


void print_help(char **argv) {
    printf("Usage: %s [options] rom_file\n", argv[0]);
    printf(" -debug \t\t\t start emulator in debug mode\n");
    printf(" -dmg   \t\t\t run emulator in dot matrix mode instead of color mode\n");
    printf(" -connect=client/server  \t run emulator as client or server mode for linking\n");
    printf(" -h     \t\t\t display this help and exit\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    
    int debug = 0;
    char *file_name = NULL;
    int dmg_mode = 0;
    ClientOrServer cs = NO_CONNECT;
    prog_name = argv[0];   
    
    if (argc < 2) {
        ARG_ERR;
    }

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-", 1) == 0) {

            if (strcmp(argv[i], "-debug") == 0) {debug = 1;}
            else if (strcmp(argv[i], "-dmg") == 0) {dmg_mode = 1;}
            else if (strcmp(argv[i], "-h") == 0) {print_help(argv);}
            else if (strcmp(argv[i], "-help") == 0) {print_help(argv);}
            else if (strncmp(argv[i], "-connect=", strlen("-connect=")) == 0) {
                cs = getConnection(argv[i] + strlen("-connect="));
                if (cs == NO_CONNECT) {
                    ARG_ERR;
                }
            }
            else {ARG_ERR;}

        } else if(i != argc - 1) {
            ARG_ERR;
        }
    }

    file_name = argv[argc - 1];

    if (!init_emu(file_name, debug, dmg_mode, cs)) {
        return 1;
    }
        
    run();
    return 0;
}

