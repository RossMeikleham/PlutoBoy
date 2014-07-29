#include "lcd.h"
#include "timers.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include "interrupts.h"
#include "graphics.h"
#include "bits.h"
#include <stdint.h>



void init_lcd() {
    current_cycles = 0;
    screen_off = 1; 
    current_lcd_stat = 0;
    HBlank_entry = 0;    
}

/* Given lcd_stat returns the new lcd_stat with the coincidence bit
 * set if there is a coincidence, unset if there isn't. Also
 * if there is a coincidence and coincidence flag is set
 * then a lcd interrupt is raised */
uint8_t check_lcd_coincidence(uint8_t const lcd_stat) {
   
    int coincidence = (get_mem(LY_REG) == get_mem(LYC_REG));
    
    // Check interrupt flag is enabled for coincidence interrupt
    // as well as an actual coincidence
    if (coincidence && (lcd_stat & BIT_6)) {
        raise_interrupt(LCD_INT);
    }
    // Set/Unset the coincidence bit in lcd_stat
    return (lcd_stat & (~0x4)) |  (coincidence << 2);
}


/*  Increment the Y Line Counter. If LY increments from 143
 *  to 144 it raises a V-Blank interrupt. LY resets after
 *  incrementing from 153. */
void inc_ly() {

    uint8_t ly = get_mem(LY_REG);
    ly = (ly + 1) % 154; //0 <= ly <= 153
    if (ly == 144) {
        raise_interrupt(VBLANK_INT);
        output_screen();
    }
      
   //Directly write ly into memory bypassing reset
   io_write_override(GLOBAL_TO_IO_ADDR(LY_REG), ly);
}   

