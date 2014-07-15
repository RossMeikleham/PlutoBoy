#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>

typedef enum {MBC0 = 0, MBC1 = 1, MBC2 = 2} MBC_MODE;


void setup_mmu(MBC_MODE mbc_mode, int rom_banks);


/*  Set and obtain 8 bit values from
 *  memory */
void set_mem(uint16_t loc, uint8_t val);
uint8_t get_mem(uint16_t loc);

/*  Set and obtain 16 bit values from
 *  memory */
void set_mem_16(uint16_t loc, uint16_t val);
uint16_t get_mem_16(uint16_t loc);

// Applies function to 8 bit value at the specified address
void mem_op(uint16_t addr, void (mem_op_fn)(uint8_t *));

#endif
