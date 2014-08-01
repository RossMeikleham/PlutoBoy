#ifndef INTERRUPTS_H
#define INTERRUPTS_H

typedef enum { VBLANK_INT = 0, LCD_INT = 1, TIMER_INT = 2, IO_INT = 3, JOYPAD_INT = 4 } InterruptCode;

/* Given an interrupt code, raises the interrupt.
 * does nothing if interrupt code supplied is invalid */
void raise_interrupt(InterruptCode ic);

/* Check if any interrupts have been raised
 * and call the appropriate Interrupt Routine if appropriate. 
 * Returns 1 if the HALT is triggered, 0 otherwise*/
int handle_interrupts();

#endif
