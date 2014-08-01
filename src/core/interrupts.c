#include "interrupts.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include "bits.h"
#include "cpu.h"



void init_interrupts() {
	interrupts[0] = { BIT_0, VBLANK_ISR_ADDR };
	interrupts[1] = { BIT_1, LCD_ISR_ADDR };
	interrupts[2] = { BIT_2, TIMER_ISR_ADDR };
	interrupts[3] = { BIT_3, IO_ISR_ADDR };
	interrupts[4] = { BIT_4, JOYPAD_ISR_ADDR };
}

/* Given an interrupt code, raises the interrupt.
 * does nothing if interrupt code supplied is invalid */
void raise_interrupt(InterruptCode ic) {
    
    if (ic < TOTAL_INTERRUPTS) { // Set flag for interrupt
        set_mem(INTERRUPT_REG, get_mem(INTERRUPT_REG) | BIT(ic));
    } 
}
