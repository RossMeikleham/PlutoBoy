#include "lcd.h"
#include "timers.h"
#include "mmu/memory.h"
#include "mmu/hdma.h"
#include "memory_layout.h"
#include "interrupts.h"
#include "graphics.h"
#include "bits.h"
#include "rom_info.h"
#include <stdint.h>


#define MAX_SL_CYCLES 456

static long current_cycles = 0;
static long current_aux_cycles = 0;
static long screen_enable_delay_cycles = 0;
static int screen_off = 0; //Stores whether screen is on or off
static int current_lcd_mode = 1;
static uint8_t stat_interrupt_signal = 0;
static uint8_t ly_counter = 144; 
static uint8_t hide_frames = 0;
static uint8_t window_line = 0;
static uint8_t vblank_line = 0;
static uint8_t scanline_transferred = 0;

int screen_enabled() {
    return !screen_off;
}

void reset_window_line() {
    uint8_t win_y = io_mem[WY_REG];
    if ((window_line == 0) && (ly_counter < 144) && (ly_counter > win_y)) {
        window_line = 144;
    }
}

void enable_screen() {
    if (screen_off) {
        screen_enable_delay_cycles = 244;
    }
}

void disable_screen() {
    screen_off = 1;
    io_mem[LY_REG] = 0;
    uint8_t stat = io_mem[STAT_REG];
    stat &= 0x7C;
    io_mem[STAT_REG] = stat;
    current_lcd_mode = 0;
    current_cycles = 0;
    current_aux_cycles = 0;
    ly_counter = 0;
    stat_interrupt_signal = 0;
}

uint8_t get_interrupt_signal() {
    return stat_interrupt_signal;
}

void set_interrupt_signal(uint8_t s) {
    stat_interrupt_signal = s;
}

int get_lcd_mode() {
    return current_lcd_mode;
}

int lcd_vblank_mode() {
    return current_lcd_mode == 1;    
}

int lcd_hblank_mode() {
    return current_lcd_mode == 0;
}

static void update_stat() {
    io_mem[STAT_REG] = (io_mem[STAT_REG] & 0xFC) | (current_lcd_mode & 0x3); 
}

/* Given lcd_stat returns the new lcd_stat with the coincidence bit
 * set if there is a coincidence, unset if there isn't. Also
 * if there is a coincidence and coincidence flag is set
 * then a lcd interrupt is raised */
void check_lcd_coincidence() {
   
    if (!screen_off) {   
        uint8_t stat = io_mem[STAT_REG];
        
        // Check we have a "coincidence" i.e. lyc == ly        
        if (io_mem[LYC_REG] == ly_counter) {
            stat |= BIT_2;
             // Check interrupt flag is enabled for a coincidence interrupt
            if (stat & BIT_6) {
                if (!stat_interrupt_signal) {
                    raise_interrupt(LCD_INT);
                }
                stat_interrupt_signal |= BIT_3;  
            } 
        } else {
            stat &= ~BIT_2;
            stat_interrupt_signal &= ~BIT_3;
        }

        io_mem[STAT_REG] = stat;
    }
}



/* Update the LCD with given number of clock cycles */
static long update_lcd(long cycles) {
      
    current_cycles += cycles;
    //int vblank = 0;
    if (!screen_off) {

    switch (current_lcd_mode) {
        case 0 : // H-Blank 

                //Can either change from H-Blank to mode 2 or
                //V-Blank when moving to next scanline
                if (current_cycles >= 204) {
                    current_cycles -= 204;
                    current_lcd_mode = 2;
                    ly_counter++;
                    io_mem[LY_REG] = ly_counter;
                    check_lcd_coincidence(); 

                    // Check if HDMA transfer needs to take place in CGB mode
                    if (cgb && (is_booting || cgb_features) && hdma_in_progress && (!halted ||
                        interrupt_about_to_raise())) {
                        long hdma_cycles = perform_hdma();
                        current_cycles += hdma_cycles;
                        cycles += hdma_cycles;

                    }

                    // H-Blank to V-Blank, change to mode 2
                    // and raise a VBLANK interrupt
                    if (io_mem[LY_REG] == 144) {
                        current_lcd_mode = 1;
                        vblank_line = 0;
                        current_aux_cycles = current_cycles;
                        raise_interrupt(VBLANK_INT);
                        
                        stat_interrupt_signal &= 0x9;
                        uint8_t stat = io_mem[STAT_REG];
                        if (stat & BIT_4) {
                            if (!((stat_interrupt_signal & BIT_0) || 
                                  (stat_interrupt_signal & BIT_3))) {
                                raise_interrupt(LCD_INT);
                            }
                            stat_interrupt_signal |= BIT_1;
                        }
                        stat_interrupt_signal &= 0x0E;

                        if (hide_frames > 0) {
                            hide_frames--;
                        } /*else {
                            vblank = 1;
                        }*/

                        window_line = 0;

                    } else {
                        stat_interrupt_signal &= 0x9;
                        uint8_t stat = io_mem[STAT_REG];
                        if (stat & BIT_5) {
                            if (stat_interrupt_signal == 0) {
                                raise_interrupt(LCD_INT);
                            }
                            stat_interrupt_signal |= BIT_2;
                        }
                        stat_interrupt_signal &= 0xE;                
                    }

                    update_stat();
                 }
                 break;

        case 1 : // V-Blank
                
                current_aux_cycles += cycles;
                if (current_aux_cycles >= 456) {
                    current_aux_cycles -= 456;
                    vblank_line++;
                    if (vblank_line <= 9) {
                        ly_counter++;
                        io_mem[LY_REG] = ly_counter;
                        check_lcd_coincidence(); 
                    }
                }
                
                // LY resets after 153
                if ((current_cycles >= 4104) && (current_aux_cycles >= 4) && (ly_counter == 153)) {
                    ly_counter = 0;
                    io_mem[LY_REG] = ly_counter;
                }

                if (current_cycles >= 4560) {
                    current_cycles -= 4560;
                    current_lcd_mode = 2;
                    update_stat();
                    stat_interrupt_signal &= 0x7;
                    check_lcd_coincidence();
                    stat_interrupt_signal &= 0xA;
                    uint8_t stat = io_mem[STAT_REG];
                    if (stat & BIT_5) {
                        if (stat_interrupt_signal == 0) {
                            raise_interrupt(LCD_INT);
                        } 
                        stat_interrupt_signal |= BIT_2;
                    }
                    stat_interrupt_signal &= 0xD;
                } 
                break;

       case 2: // OAM read
                if (current_cycles >= 80) { 
                    current_cycles -= 80;
                    current_lcd_mode = 3;
                    scanline_transferred = 0;
                    stat_interrupt_signal &= 0x8;
                    update_stat();
                }
                break;

      case 3: 
                
                // Data to LCD driver transfer
                if (!scanline_transferred && (current_cycles >= (ly_counter == 0 ? 160 : 48))) {                    
                    scanline_transferred = 1;
                    io_mem[LY_REG] = ly_counter;
                    draw_row();        
                }                

                if (current_cycles >= 172) {
                    current_cycles -= 172;
                    current_lcd_mode = 0;
                    update_stat();

                    stat_interrupt_signal &= 0x8;
                    uint8_t stat = io_mem[STAT_REG];

                    if (stat & BIT_3) {
                        if (!(stat_interrupt_signal & BIT_3)) {
                            raise_interrupt(LCD_INT);
                        }
                        stat_interrupt_signal |= BIT_0;
                    }

                }
                break;
    }
    // Screen off
    } else {
        if (screen_enable_delay_cycles > 0) {
            screen_enable_delay_cycles -= cycles;

            if (screen_enable_delay_cycles <= 0) {
                screen_enable_delay_cycles = 0;
                screen_off = 0;
                hide_frames = 3;
                current_lcd_mode = 0;
                current_cycles = 0;
                current_aux_cycles = 0;
                ly_counter = 0;
                window_line = 0;
                vblank_line = 0;
                io_mem[LY_REG] = ly_counter;
                stat_interrupt_signal = 0;

                uint8_t stat = io_mem[STAT_REG];
                if (stat & BIT_5) {
                    raise_interrupt(LCD_INT);
                    stat_interrupt_signal |= BIT_2;
                }
                check_lcd_coincidence();

            }
        }    
    }

    return cycles;
}


/* Given the elapsed cpu cycles since the last
 * call to this function, updates the internal LCD
 * modes, registers and if a Vertical Blank occurs redisplays
 * the screen. Returns 1 if an entire frame has finished drawing,
 * 0 otherwise. */
long update_graphics(long cycles) {
  
    return update_lcd(cycles);
}  
