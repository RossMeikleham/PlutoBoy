#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>
#include <stdlib.h>

typedef enum {MBC0 = 0, MBC1 = 1, MBC2 = 2} MBC_MODE;



/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(char const *file_data, size_t size);

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

// Applies function to 8 bit value at the specified address
void mem_op(uint16_t addr, void (mem_op_fn)(uint8_t *));

#endif
