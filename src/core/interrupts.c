#include "interrupts.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include "bits.h"
#include "cpu.h"

Interrupts interrupts[5] = { 
    {.flag = BIT_0, .isr_addr = VBLANK_ISR_ADDR},
    {.flag = BIT_1, .isr_addr = LCD_ISR_ADDR},
    {.flag = BIT_2, .isr_addr = TIMER_ISR_ADDR},
    {.flag = BIT_3, .isr_addr = IO_ISR_ADDR},
    {.flag = BIT_4, .isr_addr = JOYPAD_ISR_ADDR}
};

/* Given an interrupt code, raises the interrupt.
 * does nothing if interrupt code supplied is invalid */
void raise_interrupt(InterruptCode ic) {
    
    if (ic < TOTAL_INTERRUPTS) { // Set flag for interrupt
        set_mem(INTERRUPT_REG, get_mem(INTERRUPT_REG) | BIT(ic));
    } 
}
