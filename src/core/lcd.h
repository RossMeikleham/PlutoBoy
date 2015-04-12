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


extern int frame_drawn;

/* Given the elapsed cpu cycles since the last
* call to this function, updates the internal LCD
* modes, registers and if a Vertical Blank occurs redisplays
* the screen, returns amount of new cycles */
long update_graphics(long cycles);

void reset_window_line();

void enable_screen();

void disable_screen();

uint8_t get_interrupt_signal();

int get_lcd_mode();

void set_interrupt_signal(uint8_t s);

void check_lcd_coincidence();

int screen_enabled();

int lcd_hblank_mode();

#endif //LCD_H
