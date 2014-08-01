#include "../../core/emu.h"

#include <stdio.h>
#include <stdlib.h>

#define ARG_ERR printf("usage ./%s \"game\" [-d]",argv[0]); return 1

int main(int argc, char* argv[]) {
    
    int debug = 0;
    char *file_name = NULL;

    switch (argc) {
      case 3: if (argv[2][0] != '-' && argv[2][1] != 'd') {
                  ARG_ERR;
              } else { debug = 1;} 

      case 2: file_name = argv[1];
      case 1: if (argc < 2) {ARG_ERR;} break;
      default: ARG_ERR;
    }
        
    if (!init(file_name, debug)) {
        return 1;
    }
        
    run();
    return 0;
}

