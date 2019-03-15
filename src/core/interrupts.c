#include "interrupts.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include "bits.h"
#include "cpu.h"
#include "rom_info.h"

// Interrupt Service Routine Addresses
#define VBLANK_ISR_ADDR  0x40
#define LCD_ISR_ADDR     0x48
#define TIMER_ISR_ADDR   0x50
#define IO_ISR_ADDR      0x58
#define JOYPAD_ISR_ADDR  0x60

#define TOTAL_INTERRUPTS (JOYPAD_INT + 1)
#define INTERRUPTS_LEN sizeof (interrupts) / sizeof (interrupts[0])

/* For each interrupt this contains their bit no in the Interrupt Flag,
 * as well as the address in memory of their interrupt service
 * routine*/
static const struct {
    uint8_t flag; 
    uint8_t isr_addr; 
                    
} interrupts[] = { 
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
        io_mem[INTERRUPT_REG] |= BIT(ic);
    } 
}


/* Check if any interrupts have been raised
 * and call the appropriate Interrupt Routine if appropriate. 
 * Returns 1 if the HALT is triggered, 0 otherwise*/
int handle_interrupts() {

    // Get all interrupts which flag has been set and are enabled
    uint8_t if_flags = io_mem[INTERRUPT_REG];
    uint8_t possible_interrupts = if_flags & io_mem[INTERRUPT_ENABLE_REG] & 0xF;

    if (possible_interrupts != 0) {
        
        // Go through each interrupt and check if it has been raised
        for (unsigned long i = 0; i < INTERRUPTS_LEN; i++) {
           
            uint8_t flag = interrupts[i].flag;
            if ((flag & possible_interrupts) != 0) {
                //Still need to check if master override is not in place 
                if (master_interrupts_enabled()) {
                    /* Unset interrupt flag for interrupt being serviced
                     * unset master interrupts so interrupt handler routine
                     * isn't unecessarily interrupted and then call
                     * the interrupt handler */
                    io_mem[INTERRUPT_REG] = if_flags & ~flag;
                    master_interrupts_disable(); 
                    restart(interrupts[i].isr_addr);

                    halted = 0;
                    return 0;
                }
                /* If Gameboy, SGB or Gameboy pocket PC is halted
                 * and interrupts disabled, cpu is unhalted and
                 * bug in the original system causes the next
                 * 1st byte of instruction to be repeated */
                else if (halted) {
                    halted = 0;
                    if (!cgb) {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}
