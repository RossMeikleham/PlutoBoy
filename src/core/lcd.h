#ifndef LCD_H
#define LCD_H

#include "lcd.h"
#include "timers.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include "interrupts.h"
#include "graphics.h"
#include "bits.h"
#include <stdint.h>
#include "cpu.h"

#define MAX_SL_CYCLES 456

long current_cycles;
int screen_off; //Stores whether screen is on or off
int current_lcd_mode;
int current_lcd_stat;
int HBlank_entry;

void init_lcd();
uint8_t check_lcd_coincidence(uint8_t const lcd_stat);
void inc_ly();

/* Update the turned on LCD given the number of cycles, and the current lcd
 *status and control registers */
inline static void update_on_lcd(uint8_t lcd_stat, uint8_t lcd_ctrl, long cycles) {
    #define MODE2_CYCLES 80 // Mode 2 lasts from 0 -> 80 cycles
    #define MODE3_CYCLES 172 // Mode 3 lasts from  80 -> (172 + 80) cycles
    #define SET_LCD_MODE(x) (lcd_stat & (0xFF - 0x3)) | x
               
    uint8_t new_lcd_mode = current_lcd_mode;
    
    current_cycles += cycles;

    switch (current_lcd_mode) {
        case 0 : // H-Blank 
                if (HBlank_entry) { // Entering H-Blank
                    HBlank_entry = 0;  
                    draw_row();
                }
                //Can either change from H-Blank to mode 2 or
                //V-Blank when moving to next scanline
                if (current_cycles >= MAX_SL_CYCLES) {
                    current_cycles -= MAX_SL_CYCLES;
                    new_lcd_mode = 2;
                    inc_ly();
                    lcd_stat = check_lcd_coincidence(lcd_stat); 
                    // H-Blank to V-Blank, change to mode 2
                    if (get_mem(LY_REG) == 144) {
                        new_lcd_mode = 1;
                        window_line = 0;
                    }
                }
                break;

        case 1 : // V-Blank
                HBlank_entry = 1;
                //Disabling LCD can only happen during V-Blank
                if (!(lcd_ctrl & BIT_7)) { 
                    screen_off = 1;
                    set_mem(LY_REG, 0); //Reset LY by writing to it
                    lcd_stat = check_lcd_coincidence(lcd_stat);
                    new_lcd_mode = 0;
                } 
                
                else if (current_cycles >= MAX_SL_CYCLES) {
                    current_cycles -= MAX_SL_CYCLES;
                    inc_ly();
                    lcd_stat = check_lcd_coincidence(lcd_stat); 
                    
                    if (get_mem(LY_REG) == 0) { //V-Blank over
                        new_lcd_mode = 2;
                        //lcd_stat = check_lcd_coincidence(lcd_stat);
                    }
                }
                break;
       case 2: // OAM read
                HBlank_entry = 1;
                if (current_cycles >= MODE2_CYCLES) { 
                    new_lcd_mode = 3;
                }
                break;

      case 3: 
                HBlank_entry = 1;
                if (current_cycles >= 252) {//MODE2_CYCLES + MODE3_CYCLES) {
                    new_lcd_mode = 0;    
                }
    }

    // Check if LCD interrupt needs to be raised from mode change
    if (new_lcd_mode != 3 && (current_lcd_mode != new_lcd_mode)) {
       
        if (lcd_stat & (1 << (new_lcd_mode + 3))) { //Check interrupt bit set
            raise_interrupt(LCD_INT);
        }
    }
    lcd_stat = SET_LCD_MODE(new_lcd_mode);
    set_mem(STAT_REG, lcd_stat);
    current_lcd_stat = lcd_stat;
    current_lcd_mode = new_lcd_mode;
}


/* Given the elapsed cpu cycles since the last
 * call to this function, updates the internal LCD
 * modes, registers and if a Vertical Blank occurs redisplays
 * the screen */
inline static void update_graphics(long cycles) {
 
    // Lower 3 bits of stat are read only 
    current_lcd_stat = (get_mem(STAT_REG) & (~7)) | (current_lcd_stat & 0x7);
    uint8_t lcd_stat = current_lcd_stat;
    uint8_t lcd_ctrl = get_mem(LCDC_REG);

    // Turning screen from off to on
    if (screen_off && (lcd_ctrl & BIT_7))  {
        screen_off = 0;
        current_lcd_mode = 2;
        lcd_stat = (lcd_stat & (~3)) +  2; // Mode 2
        current_cycles = 4;
        update_timers(current_cycles);
    } 

    if (!screen_off) {
        update_on_lcd(lcd_stat, lcd_ctrl, cycles);
    } 
}  

#endif //LCD_H
