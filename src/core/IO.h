#ifndef IO_H
#define IO_H

#include <stdint.h>


void set_lcd_interrupt();

int check_interrupts();

void io_set_mem(uint8_t io_addr, uint8_t val);
uint8_t io_get_mem(uint8_t io_addr);

    
void increment_tima();
void increment_div();
void increment_ly();


#endif /*  IO_H */
