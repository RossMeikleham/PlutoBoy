#ifndef LCD_H
#define LCD_H

/* Given the elapsed cpu cycles since the last
 * call to this function, updates the internal LCD
 * modes, registers and if a Vertical Blank occurs redisplays
 * the screen */
void update_graphics(long cycles);

#endif //LCD_H
