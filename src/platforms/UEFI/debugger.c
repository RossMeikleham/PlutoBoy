#include "../../non_core/debugger.h"

#include "../../core/mmu/memory.h"
#include "../../core/disasm.h"
#include "../../core/cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  Check if first time entering debugger and prints
 *  welcome message if it is */
//static void check_first_time() {
//}
    
/* Checks if exit command was entered, and aborts
 * the program if it has. */
//static void check_exit(char *buf) {
//}


/* Chcecks if a show registers command has been entered,
 * prints out the contents of the CPU registers if it has and
 * returns 1. Otherwise returns 0 */
//static int show_regs(char *buf) {
//    return 0;
//}


/*  Check if a valid step command has been entered,
 *  sets the current step count if it has and return 1.
 *  Returns 0 otherwise */
//static int check_step(char *buf) {
//    return 0;    
//}


/* Returns 1 if a continue command has been entered,
 * 0 otherwise */
//static int check_continue(char *buf) {
//    return 0;
//}


/* Check if a help command has been entered, display 
 * a list of all commands and their details if it has and return .
 * Return 0 otherwise */
//static int help(char *buf) {
//    return 0;
//}


/* Check if a set breakpoint command has been entered,
 * and set a breakpoint to the given hex memory address if valid.
 * Returns 1 if command was entered, 0 otherwise */
//static int check_breakpoint(char *buf) {
//    return 0;
//}


/* Check if a show memory command has been entered,
 * and display the contents of the given hex memory location
 * if valid. Returns 1 if command was entered, 0 otherwise */
//static int check_show_mem(char *buf) {
//     return 0;
//}


/* Check if a dissasemble command has been entered,
 * and disassemble the given hex memory location if valid.
 * Returns 1 if command was entered, 0 otherwise */
///static int check_disasm(char *buf) {
//    return 0;
//}


/* Get debugger command(s) and perform a number
 * of debugger actions. Returns an integer containing set/unset
 * flags specifying the options selected. */
int get_command() {
   return 0; 
}



/* Get number of steps, 0 or below
 * if stepping not occuring */
long get_steps() {
    return 0;
}

// No longer stepping
void turn_steps_off() {
}


/* Get 16bit address of breakpoint
 * result is below 0 if no breakpoint set */
long get_breakpoint() {
    return 0;
}

//Disable breakpoint
void turn_breakpoint_off() {
}



