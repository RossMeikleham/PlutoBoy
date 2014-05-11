/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/02/14 13:29:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ross Meikleham, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cpu.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>
#include "graphics.h"
#include <string.h>
#include <stdlib.h>

#define BREAKPOINT_OFF 0x100009
#define BREAKPOINT_MAX 0xFFFF
#define BREAKPOINT_MIN 0x0

int DEBUG = 0;
int STEP_COUNT = -1;
int BREAKPOINT = BREAKPOINT_OFF;

/* Stores possible frequencies in hz of when the
 * timer should be incremented */
long timer_frequencies[] = {4096, 16384, 65536, 262144};

long timer_frequency;
long clock_speed;


 int load_program(const char *filename)
  {
   
      uint16_t count;
      uint8_t cur;
      FILE *file;
  
      //open file in binary read mode
      //read byte by byte into memory
      if(!(file = fopen(filename,"rb"))) {
          fprintf(stderr, "Error opening file %s\n", filename);
          return 0;
     }
  
      for (count = 0; count <= 0xFFFF; count++) {
      //Read file contents into buffer
        if(!fread(&cur, 1, 1, file)) {
            break;
        }
        set_mem(count, cur);
      }
      fclose(file);
      return 1;
  }


void get_command() {
    #define BUFSIZE 1024
    int step;
    int breakpoint;
    char buf[BUFSIZE];

    BREAKPOINT = BREAKPOINT_OFF;
    STEP_COUNT = -1;

    for(;;) {
   
        printf("->");
        fgets(buf, BUFSIZE, stdin);
        
        if(!strcmp(buf, "exit\n")) {
            exit(EXIT_SUCCESS);
        }
        if(!strcmp(buf, "showregs\n")) {
            print_regs();
        }
        else if (BUFSIZE > 5 && !strncmp(buf, "step ", 5)) {
            if (sscanf(buf+5, "%d", &step) && step > 0) {
                STEP_COUNT = step;
                return;
            } else {
                printf("usage: step [stepcount] where stepcount > 0\n");
            }
        }
        else if (!strcmp(buf, "go\n")) {
            return;
        }
        else if (BUFSIZE > 5 && !strncmp(buf,"setb ",5)) {
            if(sscanf(buf+5, "%d", &breakpoint) && breakpoint >= BREAKPOINT_MIN && breakpoint <= BREAKPOINT_MAX) {
                BREAKPOINT = breakpoint;
            } else {
                printf("usage bpoint [breakpoint] where breakpoint is between 0x0000 and 0xFFFF inclusive\n");
            } 
        } 
        else {printf("unknown command\n");}
   }
    
}


int run(long cycles) {
    long ly_cycles = 0, current_cycles;
    int num;
    get_command();
    while(cycles > 0) {
       // check_interrupt();

        /*  Increment lcd y line every 456 cycles */
        if(ly_cycles >= 456) {
          //  increment_ly();
            ly_cycles = 0;
        }
        current_cycles = exec_opcode();
        cycles -= current_cycles;
        ly_cycles += current_cycles;
        //scanf("%d",&num);
        if (STEP_COUNT > 0 && --STEP_COUNT == 0) {
            get_command();
        }

    }

    return 0;
}


void timer_interrupt() {
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("usage ./gb game");
        return 1;
    }
   
    if( !load_program(argv[1])){
        printf("failed");
        return 1;
    }
    
    uint16_t i;
    /*  print rom name */
    printf("Game Title:");
    for(i = ROM_NAME_START; i<= ROM_NAME_END; i++) {
        printf("%c",get_mem(i));
    }
   
    printf("\nLicensee: %s", get_licensee());
    printf("\nDestination: %s", get_destination_code(get_mem(DESTINATION_CODE)));
    printf("\nROM size: %dKB",get_rom_size(get_mem(CARTRIDGE_ROM_SIZE)));
    printf("\nRAM save size: %dKB",get_ram_save_size(get_mem(CARTRIDGE_RAM_SIZE)));
    printf("\nCartridge Type: %s",get_cartridge_type());
    printf("\nGameboy Color Only Game:%s", is_colour_compatible() ? "Yes":"No");
    printf("\nSuper Gameboy Features:%s", has_sgb_features() ? "Yes":"No");
    printf("\n");
    
    init_gfx();
    reset_cpu();

    run(100000000);

}

