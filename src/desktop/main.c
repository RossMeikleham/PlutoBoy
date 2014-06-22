
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

#include "../core/cpu.h"
#include "../core/memory.h"
#include "../core/disasm.h"
#include <stdint.h>
#include <stdio.h>
#include "../core/graphics.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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

/* Performs set of debugging commands  
 * TODO seperate functionality and split
 * into smaller functions */
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
        else if(!strcmp(buf, "dumptile0\n")) {
            unsigned int i;
            for (i = 0; i < 256; i++) {
                draw_tile_0(i,i % 16, i / 16);
            }
        }
        else if(!strcmp(buf, "dumptile1\n")) {
            int i;
            for (i = -128; i < 128; i++) {
                
                draw_tile_1(i, (i+128)%16, (i+128)/16);
            }
        } else if (!strcmp(buf,"dumpsprites\n")) {
            draw_sprites();
        }
        else if (!strcmp(buf, "dumpbg0\n")) {
            draw_background_0();
        }
        else if (!strcmp(buf, "dumpbg1\n")) {
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
        else if (!strcmp(buf, "help\n") || !strcmp(buf, "h\n")) {
            printf("exit :      exit emulator\n"
                   "showregs:    display current contents of registers\n"
                   "dumptile0:   display tile set 0 from vram\n"
                   "dumptile1:   display tile set 1 from vram\n"
                   "dumpbg0:     display background set 0 from vram\n"
                   "dumpbg1:     display background set 1 from vram\n"
                   "dumpsprites: display sprites from OAM\n"
                   "step [n]:    execute n operations\n"
                   "go:          execute forever\n"
                   "setb [n]:    set a breakpoint for address n\n"
                   "showmem [n]: display contents of memory address n\n"
                   "disasm [n]:  dissasemble instruction at address n \n");

        }
        else if (BUFSIZE > 5 && !strncmp(buf,"setb",4)) {
            if(sscanf(buf+5, "%d", &breakpoint) && breakpoint >= BREAKPOINT_MIN && breakpoint <= BREAKPOINT_MAX) {
                BREAKPOINT = breakpoint;
            } else {
                printf("usage: setb [breakpoint] (where breakpoint is between 0x0000 and 0xFFFF inclusive)\n");
            } 
        }
        else if (BUFSIZE > 8 && !strncmp(buf,"showmem",7)) {
            int mem;
            if(sscanf(buf+8, "%d", &mem) == 1 && mem >= 0 && mem <= 0xFFFF) {
                printf("0x%X\n",get_mem(mem));
            } else {
                printf("usage: showmem [address] (where address is between 0x0000 and 0xFFFF inclusive)\n"); 
            }
        }
        else if (BUFSIZE > 7 && !strncmp(buf,"disasm",6)) {
            int mem;
            if(sscanf(buf+7, "%d", &mem) == 1 && mem >= 0 && mem <= 0xFFFF) {
                dasm_instruction(mem, stdout);
                printf("\n");
            } else {
                printf("usage: disasm [address] (where address is between 0x0000 and 0xFFFF inclusive)\n");
            }
        }
        else if (!strcmp(buf,"\n")) {
            ;;
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
            //increment_ly();
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
    
    for(;;)
        run(100000000);

}

