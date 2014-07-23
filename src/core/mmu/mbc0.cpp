#include "mbc0.h"

#include <stdint.h>

void MBC0::write_mem(uint16_t addr, uint8_t val) {
    /* No memory bank chip so just attempt to
       write to ordinary memory */
    write_ordinary_mem(addr, val);
}

uint8_t MBC0::read_mem(uint16_t addr) {
    // Reading from first 2 fixed ROM banks
    if (addr < 0x8000) {
        return ROM_banks[addr / 0x4000][addr - 0x4000];
    }
    // Anywhere else in memory
    read_ordinary_mem(addr);
}
