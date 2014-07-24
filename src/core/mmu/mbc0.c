#include "mbc.h"
#include "memory.h"

/* MEMORY BANK CONTROLLER 0 
 * 
 * For 32kb roms, maps: 
 *  bank 0 to memory locations 0x0000 - 0x3FFF
 *  bank 1 to memory locations 0x4000 - 0x7FFF 
 *
 *  Writing to any of the two banks has no effect.                     
 * */

uint8_t read_MBC0(uint16_t addr) {
    
    if (addr <= 0x8000) {
        return ROM_banks[0][addr];
    }
    
    return 0x0;
}


void write_MBC0(uint16_t addr, uint8_t val) {
    return;
}
