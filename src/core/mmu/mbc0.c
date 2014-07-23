#include "mbc.h"
#include "memory.h"


uint8_t read_MBC0(uint16_t addr) {
    
    if (addr <= 0x8000) {
        return ROM_banks[0][addr];
    }
    
    return 0x0;
}


void write_MBC0(uint16_t addr, uint8_t val) {
    return;
}
