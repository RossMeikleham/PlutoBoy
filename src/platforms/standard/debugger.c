#include "../../non_core/debugger.h"

#include "../../core/mmu/memory.h"
#include "../../core/disasm.h"
#include "../../core/cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BREAKPOINT_MIN 0x000
#define BREAKPOINT_MAX 0xFFFF

#define BUFSIZE 1024

static long breakpoint = -1;
static long steps = 0;
static int first_time = 1;
static int flags = 0;


/*  Check if first time entering debugger and prints
 *  welcome message if it is */
static void check_first_time() {
    
    if (first_time) {
        printf("Welcome to the debugger, enter \"h\" or \"help\""\
            " for a list of options\n");
        first_time = 0;
    }
}

/* Checks if exit command was entered, and aborts
 * the program if it has. */
static void check_exit(char *buf) {

    if(!strcmp(buf, "exit\n")) {
        exit(EXIT_SUCCESS);
    }
}


/* Chcecks if a show registers command has been entered,
 * prints out the contents of the CPU registers if it has and
 * returns 1. Otherwise returns 0 */
static int show_regs(char *buf) {

    if(!strcmp(buf, "showregs\n")) {
        print_regs();
        return 1;
    }
    return 0;
}


/*  Check if a valid step command has been entered,
 *  sets the current step count if it has and return 1.
 *  Returns 0 otherwise */
static int check_step(char *buf) {

    if (BUFSIZE > 5 && !strncmp(buf, "step ", 5)) {
        long step;
        if (sscanf(buf+5, "%ld", &step) && step > 0) {
            steps = step;
            flags |= STEPS_SET;
            return 1;           
        } else {
            printf("usage: step [stepcount] where stepcount > 0\n");
            return 0;
        }
    }
    return 0;    
}


/* Returns 1 if a continue command has been entered,
 * 0 otherwise */
static int check_continue(char *buf) {

    return (!strcmp(buf, "continue\n"));

}


/* Check if a help command has been entered, display 
 * a list of all commands and their details if it has and return .
 * Return 0 otherwise */
static int help(char *buf) {

    if (!strcmp(buf, "help\n") || !strcmp(buf, "h\n")) {
        printf("exit :       exit emulator\n"
               "showregs:    display current contents of registers\n"
               "step [n]:    execute n operations\n"
               "continue:    execute forever\n"
               "setb [n]:    set a breakpoint for address n\n"
               "showmem [n]: display contents of memory address n\n"
               "disasm [n]:  dissasemble instruction at address n \n");
        return 1;
    }
    return 0;
}


/* Check if a set breakpoint command has been entered,
 * and set a breakpoint to the given hex memory address if valid.
 * Returns 1 if command was entered, 0 otherwise */
static int check_breakpoint(char *buf) {
   
    if (BUFSIZE > 5 && !strncmp(buf,"setb",4)) {
        int bp = -1;
        if (sscanf(buf+5, "%x", &bp) && bp >= BREAKPOINT_MIN && bp <= BREAKPOINT_MAX) {
            breakpoint = bp;
            flags|=BREAKPOINT_SET;
            return 1;
        } else {
            printf("usage: setb [breakpoint]"\
            "(where breakpoint is between 0x0000 and 0xFFFF inclusive)\n");
            return 0;
        }
    }
    return 0;
}


/* Check if a show memory command has been entered,
 * and display the contents of the given hex memory location
 * if valid. Returns 1 if command was entered, 0 otherwise */
static int check_show_mem(char *buf) {

    if (BUFSIZE > 8 && !strncmp(buf,"showmem",7)) {
        int mem;
        if(sscanf(buf+8, "%x", &mem) == 1 && mem >= 0 && mem <= 0xFFFF) {
            printf("0x%X\n",get_mem(mem));      
            return 1; 
        }
         else {
            printf("usage: showmem [address]"\
            "(where address is between 0x0000 and 0xFFFF inclusive)\n"); 
            return 0;
        }
     }
     return 0;
}


/* Check if a dissasemble command has been entered,
 * and disassemble the given hex memory location if valid.
 * Returns 1 if command was entered, 0 otherwise */
static int check_disasm(char *buf) {
        
    if (BUFSIZE > 7 && !strncmp(buf,"disasm",6)) {
        int mem;
        if(sscanf(buf+7, "%x", &mem) == 1 && mem >= 0 && mem <= 0xFFFF) {
            dasm_instruction(mem, stdout);
            printf("\n");
            return 1;
        } else {
            printf("usage: disasm [address]"\
            "(where address is between 0x0000 and 0xFFFF inclusive)\n");
            return 0;
        }
    }
    return 0;
}


/* Get debugger command(s) and perform a number
 * of debugger actions. Returns an integer containing set/unset
 * flags specifying the options selected. */
int get_command() {

    flags = 0;
    char buf[BUFSIZE];
    
    check_first_time();

    for(;;) {
   
        printf("->");
        fgets(buf, BUFSIZE, stdin);
    
        check_exit(buf);     
        if (check_continue(buf) || check_step(buf)) {
            return flags;
        }
        
        // If no valid options selected, show error message
        if (!(check_show_mem(buf) || check_breakpoint(buf) || check_disasm(buf)
            || help(buf) || show_regs(buf) || !strcmp(buf, "\n"))) {
            printf("unknown command\n");
        }
        
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



