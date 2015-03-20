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
* the screen, returns 1 if a frame has finished drawing,
* 0 otherwise */
void update_graphics(long cycles);

#endif //LCD_H
