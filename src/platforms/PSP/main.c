#include "../../core/emu.h"
#include "../../core/serial_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_ERR printf("usage ./%s \"game\" [-d] [client/server port]",argv[0]); return 1

ClientOrServer getConnection(int count, char *argv[]) {
    if (!strcmp(argv[count],"client")) {return CLIENT;}
    else if(!strcmp(argv[count], "server")) {return SERVER;}
    else return NO_CONNECT;
}


int main(int argc, char* argv[]) {
    
    int debug = 0;
    char *file_name = NULL;
    ClientOrServer cs = NO_CONNECT;

    switch (argc) {
      case 4: if ((cs = getConnection(3, argv)) == NO_CONNECT) {ARG_ERR;}
              
      case 3: if (argv[2][0] == '-' && argv[2][1] == 'd') {debug = 1;}
              else if ((cs = getConnection(2, argv)) == NO_CONNECT) {ARG_ERR;}
                
      case 2: file_name = argv[1];
      case 1: if (argc < 2) {ARG_ERR;} break;
      default: ARG_ERR;
    } 

    if (!init(file_name, debug, cs)) {
        return 1;
    }
        
    run();
    return 0;
}

