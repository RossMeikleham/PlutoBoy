/*
 * =====================================================================================
 *
 *       Filename:  debugger.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/14 16:51:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../../non_core/debugger.h"

#include "../../core/memory.h"
#include "../../core/disasm.h"
#include "../../core/cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BREAKPOINT_MIN 0x000
#define BREAKPOINT_MAX 0xFFFF

static long breakpoint = -1;
static long steps = 0;
static int first_time = 1;
/* Get debugger command
 * and perform debugger action */
Command get_command() {
    #define BUFSIZE 1024
    int flags;
    char buf[BUFSIZE];
    
    if (first_time) {
        printf("Welcome to the debugger, enter \"h\" or \"help\""\
            " for a list of options\n");
        first_time = 0;
    }
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
            long step;
            if (sscanf(buf+5, "%ld", &step) && step > 0) {
                steps = step;
                return flags|=STEPS_SET;
            } else {
                printf("usage: step [stepcount] where stepcount > 0\n");
            }
        }
        else if (!strcmp(buf, "continue\n")) {
            return flags;
        }
        else if (!strcmp(buf, "help\n") || !strcmp(buf, "h\n")) {
            printf("exit :       exit emulator\n"
                   "showregs:    display current contents of registers\n"
                   "step [n]:    execute n operations\n"
                   "continue:    execute forever\n"
                   "setb [n]:    set a breakpoint for address n\n"
                   "showmem [n]: display contents of memory address n\n"
                   "disasm [n]:  dissasemble instruction at address n \n");

        }
        else if (BUFSIZE > 5 && !strncmp(buf,"setb",4)) {
            int bp = -1;
            if(sscanf(buf+5, "%d", &bp) && bp >= BREAKPOINT_MIN && bp <= BREAKPOINT_MAX) {
                breakpoint = bp;
                flags|=BREAKPOINT_SET;
            } else {
                printf("usage: setb [breakpoint] (where breakpoint is between 0x0000 and 0xFFFF inclusive)\n");
            } 
        }
        else if (BUFSIZE > 8 && !strncmp(buf,"showmem",7)) {
            int mem;
            if(sscanf(buf+8, "%x", &mem) == 1 && mem >= 0 && mem <= 0xFFFF) {
                printf("mem for %x:\n",mem);
                for (int y = 0; y < 32; y++) {
                    printf("%d: ",y);
                    for(int x = 0; x < 32; x++) {
                        printf("$%02X ",get_mem((32 * y) + x + mem));
                    }
                    printf("\n");
                }
                //printf("0x%X\n",get_mem(mem));
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

   return NONE; 
}


/* Get number of steps, 0 or below
 * if stepping not occuring */
long get_steps() {
    return steps;
}

// No longer stepping
void turn_steps_off() {
    steps = 0;
}


/* Get 16bit address of breakpoint
 * result is below 0 if no breakpoint set */
long get_breakpoint() {
    return breakpoint;
}

//Disable breakpoint
void turn_breakpoint_off() {
    breakpoint = -1;
}



