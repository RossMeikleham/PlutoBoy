#include "../../core/sound.h"

// UEFI doesn't have any sound support, just stub everything out


void init_apu() {
}
    
void sound_add_cycles(unsigned c) {
}

void write_apu(uint16_t addr, uint8_t val) {
}

uint8_t read_apu(uint16_t addr) {
    return 0xFF;
}

void end_frame() {
}                           
