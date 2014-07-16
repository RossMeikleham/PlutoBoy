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
#include <stdint.h>
#include <stdio.h>

#define MAX_SL_CYCLES 452

static long scanline_counter = 0;


// Given lcd_stat returns the new lcd_stat with the coincidence bit
// set if there is a coincidence, unset if there isn't. Also
// if there is a coincidence and coincidence flag is set
// then a lcd interrupt is raised
static inline uint8_t check_lcd_coincidence(uint8_t const lcd_stat) {
   
    int coincidence = //LY == LYC
        io_get_mem(GLOBAL_TO_IO_ADDR(LY_REG)) == 
        io_get_mem(GLOBAL_TO_IO_ADDR(LYC_REG));
    
    if (coincidence) {
        set_lcd_interrupt();
    }
    // Set/Unset the coincidence bit in lcd_stat
    return (lcd_stat & (0xFF - 0x4)) |  (coincidence << 2);
}


static void update_on_lcd(uint8_t lcd_stat) {

    #define MODE2_CYCLES 80
    #define MODE3_CYCLES 172
    #define MODE0_CYCLES MAX_SL_CYCLES - MODE3_CYCLES - MODE2_CYCLES

    #define SET_LCD_MODE(x) (lcd_stat & (0xFF - 0x3)) | x
               
    uint8_t lcd_mode = lcd_stat & 2;
    uint8_t current_mode = 0;
    uint8_t ly = io_get_mem(GLOBAL_TO_IO_ADDR(LY_REG));
    int interrupt_enabled = 0; /* Check whether if mode changed and IE bit in
                                  status for that mode is set */ 

    // V-Blank Period, need to set Mode 1
    if (ly >= 144) {
        
        current_mode = 1; 
        lcd_stat = SET_LCD_MODE(1);       
        interrupt_enabled = lcd_stat & BIT_4;

    // Currently in mode 2
    } else if (scanline_counter < MODE2_CYCLES) {

        current_mode = 2;
        lcd_stat = SET_LCD_MODE(2);
        interrupt_enabled = lcd_stat & BIT_5;

    // Currently in mode 3
    } else if (scanline_counter < MODE3_CYCLES) {

        current_mode = 3;
        lcd_stat |= 3;
    
    // Currently in mode 0
    } else {

        current_mode = 0;
        lcd_stat = SET_LCD_MODE(0);
        interrupt_enabled = lcd_stat & BIT_3;
    } 

    // If mode changed and interrupt for that mode is enabled
    // raise an LCD interrupt
    if ((current_mode != lcd_mode) && interrupt_enabled) {
        set_lcd_interrupt();
    }
    
    lcd_stat = check_lcd_coincidence(lcd_stat);

    io_set_mem(GLOBAL_TO_IO_ADDR(STAT_REG), lcd_stat);
}



void update_lcd() {

    uint8_t lcd_stat = io_get_mem(GLOBAL_TO_IO_ADDR(STAT_REG));
    uint8_t lcd_ctrl = io_get_mem(GLOBAL_TO_IO_ADDR(LCDC_REG));
     
    if (!(lcd_ctrl & BIT_7)) { //LCD off
        scanline_counter = 0; 
        io_set_mem(GLOBAL_TO_IO_ADDR(LY_REG), 0); //Reset LY by writing to it

        //Set first 2 bits in lcd_stat to 01 i.e. mode 1
        lcd_stat &= (0xFF - 0x3);
        lcd_stat |=  0x1; 
        io_set_mem(GLOBAL_TO_IO_ADDR(STAT_REG), lcd_stat);

    } else { //LCD on
        update_on_lcd(lcd_stat);
    }
}



void update_graphics(long cycles) {

    uint8_t lcd_ctrl = io_get_mem(GLOBAL_TO_IO_ADDR(LCDC_REG));
    
    update_lcd();

    if (lcd_ctrl & BIT_7) { //LCD on   
        scanline_counter += cycles;

        while (scanline_counter >= MAX_SL_CYCLES) {
            scanline_counter = 0;
            increment_ly();
        }
    }
}



