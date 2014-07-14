/* Ross Meikleham 2014*/                  
#ifndef CPU_H
#define CPU_H

#include <stdint.h>

/*  Call interrupt handler code */
void restart(uint8_t addr);

/*  Check if master interrupts are enabled */
int master_interrupts_enabled();
void master_interrupts_disable();
void master_interrupts_enable();

int is_halted();
void unhalt_cpu();
void reset_cpu();


/*  Executes current instruction and returns
 *  the number of machine cycles it took */
int exec_opcode(int skip_bug);


void print_regs();


#endif
