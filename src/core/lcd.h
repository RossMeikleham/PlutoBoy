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

uint8_t check_lcd_coincidence(uint8_t const lcd_stat);
void inc_ly();

/* Given the elapsed cpu cycles since the last
* call to this function, updates the internal LCD
* modes, registers and if a Vertical Blank occurs redisplays
* the screen */
void update_graphics(long cycles);

#endif //LCD_H
