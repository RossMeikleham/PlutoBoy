#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C"
{
#endif


/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(unsigned char const *file_data, size_t size);

/*  Set and obtain 8 bit values from
 *  memory */
void set_mem(uint16_t loc, uint8_t val);
uint8_t get_mem(uint16_t loc);

/* Directly inject a value in memory without performing
 * checks, use carefully. Used for fast access or for
 * controllers which have direct access to that location
 * in memory in which the CPU does not*/
void set_mem_override(uint16_t loc, uint8_t val); 

/*  Set and obtain 16 bit values from
 *  memory */
void set_mem_16(uint16_t loc, uint16_t val);
uint16_t get_mem_16(uint16_t loc);



#ifdef __cplusplus
}
#endif


#endif
