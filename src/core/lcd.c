/*
 * =====================================================================================
 *
 *       Filename:  lcd.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/07/14 13:45:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "lcd.h"
#include "IO.h"
#include "memory.h"
#include "graphics.h"
#include <stdint.h>
#include <stdio.h>

#define MAX_SL_CYCLES 456

static long current_cycles = 0;
static int screen_off = 1; //Stores whether screen is on or off
static int current_lcd_mode;

// Given lcd_stat returns the new lcd_stat with the coincidence bit
// set if there is a coincidence, unset if there isn't. Also
// if there is a coincidence and coincidence flag is set
// then a lcd interrupt is raised
static inline uint8_t check_lcd_coincidence(uint8_t const lcd_stat) {
   
    int coincidence = //LY == LYC
        io_get_mem(GLOBAL_TO_IO_ADDR(LY_REG)) == 
        io_get_mem(GLOBAL_TO_IO_ADDR(LYC_REG));
    
    // Check interrupt flag is enabled for coincidence interrupt
    // as well as an actual coincidence
    if (coincidence && (lcd_stat & BIT_6)) {
        set_lcd_interrupt();
    }
    // Set/Unset the coincidence bit in lcd_stat
    return (lcd_stat & (~0x4)) |  (coincidence << 2);
}


static inline void update_on_lcd(uint8_t lcd_stat, uint8_t lcd_ctrl, long cycles) {
    #define MODE2_CYCLES 80
    #define MODE3_CYCLES 172
    #define SET_LCD_MODE(x) (lcd_stat & (0xFF - 0x3)) | x
               
    static int HBlank_entry = 0;
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
                    increment_ly();
                    lcd_stat = check_lcd_coincidence(lcd_stat); 
                    // H-Blank to V-Blank, change to mode 2
                    if (get_mem(LY_REG) == 144) {
                        new_lcd_mode = 1;
                    }
                }
                break;

        case 1 : // V-Blank
                HBlank_entry = 1;
                
                if (!(lcd_ctrl & BIT_7)) { //Disable lcd
                    screen_off = 1;
                    set_mem(LY_REG, 0); //Reset LY by writing to it
                    lcd_stat = check_lcd_coincidence(lcd_stat);
                    new_lcd_mode = 0;
                    current_cycles = 0;
                } 
                
                else if (current_cycles >= MAX_SL_CYCLES) {
                    current_cycles -= MAX_SL_CYCLES;
                    increment_ly();
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
            set_lcd_interrupt();
        }
    }
    lcd_stat = SET_LCD_MODE(new_lcd_mode);
    set_mem(STAT_REG, lcd_stat);
    current_lcd_mode = new_lcd_mode;
}




void update_graphics(long cycles) {
  
    uint8_t lcd_stat = get_mem(STAT_REG);
    uint8_t lcd_ctrl = get_mem(LCDC_REG);

    // Turning screen from off to on
    if (screen_off && (lcd_ctrl & BIT_7))  {
        screen_off = 0;
        current_lcd_mode = 2;
        lcd_stat = (lcd_stat & (~3)) +  2; // Mode 2
    } 

    if (!screen_off) {
        update_on_lcd(lcd_stat, lcd_ctrl, cycles);
    }
}
  



