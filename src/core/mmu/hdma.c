//HDMA CGB module
#include <stdint.h>

#include "hdma.h"
#include "memory.h"

#include "../memory_layout.h"
#include "../timers.h"

static uint16_t source_addr;
static uint16_t dest_addr;
static unsigned bytes;

void start_gbc_dma(uint16_t s_addr, uint16_t e_addr, unsigned b, int type) { 
    source_addr = s_addr;
    dest_addr = e_addr;
    bytes = b * 0x10;
    hdma_in_progress = type;
    gdma_in_progress = !type;
    bytes_transferred = 0;
}


// Performs a HDMA transfer of 0x10 bytes from source to destination
// returns the amount of machine cycles taken
long perform_hdma() {
    
    uint16_t source = source_addr & 0xFFF0;
    uint16_t dest = (dest_addr & 0x1FF0) | 0x8000;

    for (int i = 0; i < 0x10; i++) {
        set_mem(source + i, get_mem(dest + 1));       
    }

    source_addr = source + 0x10;
    dest_addr = dest + 0x10;

    //Keep destination address between 0x8000 and 0x9FFF
    if (dest_addr >= 0xA000) {
        dest_addr -= 0x2000;
    }

    // Keep source address away from destination addresses
    if (source_addr >= 0x8000) {
        source_addr += 0x2000;
    }

    set_mem(HDMA1_REG, source_addr >> 8); 
    set_mem(HDMA2_REG, source_addr & 0xFF);
    set_mem(HDMA3_REG, dest_addr >> 8);
    set_mem(HDMA4_REG, dest_addr & 0xFF);
    set_mem(HDMA5_REG, get_mem(HDMA5_REG) - 1); // 1 less block to transfer

    bytes -= 0x10;

    return (get_clock_speed() == CGB_CLOCK_SPEED_HZ ? 17 : 9) * 4;
}
