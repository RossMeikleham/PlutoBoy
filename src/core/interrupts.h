#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define TOTAL_INTERRUPTS (JOYPAD_INT + 1)
typedef enum {VBLANK_INT, LCD_INT, TIMER_INT, IO_INT, JOYPAD_INT} InterruptCode;

/* Given an interrupt code, raises the interrupt.
 * does nothing if interrupt code supplied is invalid */
void raise_interrupt(InterruptCode ic);

/*Check if any interrupts have been raised
 * and call the appropriate Interrupt Routine if appropriate. 
 * Returns 1 if the HALT is triggered, 0 otherwise*/
int handle_interrupts(); 

#endif
