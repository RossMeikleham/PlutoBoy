#include "mbc.h"
#include "mbc0.h"

#include "../romInfo.h"

#include "../../non_core/logger.h"

static MBC *memory_bank_controller;


/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(unsigned char const *file_data, unsigned long size) {
    
    if (size < CARTRIDGE_ROM_SIZE + 1) {
        log_message(LOG_ERROR, "Cartridge size is too small (%lu bytes)\n",size);
        return 0;
    }

    unsigned long rom_size = 
        get_rom_size(file_data[CARTRIDGE_ROM_SIZE]) * 1024;

    // Data read in doesn't match header information
    if (size != rom_size) {
        log_message(LOG_ERROR, "Cartridge header info on its size (%lu bytes) " \
            "doesn't match file size (%lu bytes)\n",rom_size, size);
        return 0;
    }

    unsigned mbc_mode = file_data[CARTRIDGE_TYPE];
    
    if (!mbc_mode) {
        //Create MBC0
        memory_bank_controller = new MBC0(file_data, size);

    } else if (mbc_mode <= 3 && mbc_mode >= 1 ) {
        //Create MBC1
    } else { log_message(LOG_ERROR, "Unimplemented memory banking for cartridge " \
                "type %u\n",mbc_mode); 
            return 0;
    }

    return 1;
}

/*  Set and obtain 8 bit values from
 *  memory */
void set_mem(uint16_t addr, uint8_t val) {
    memory_bank_controller->write_mem(addr, val);
}
uint8_t get_mem(uint16_t addr) {
    return memory_bank_controller->read_mem(addr);
}

/* Directly inject a value in memory without performing
 * checks, use carefully. Used for fast access or for
 * controllers which have direct access to that location
 * in memory in which the CPU does not*/
//void set_mem_override(uint16_t loc, uint8_t val); 

/*  Set and obtain 16 bit values from
 *  memory */
void set_mem_16(uint16_t addr, uint16_t val) {
    memory_bank_controller->write_mem(addr, val & 0xFF);
    memory_bank_controller->write_mem(addr + 1, val >> 8); 
}

uint16_t get_mem_16(uint16_t addr){
    return memory_bank_controller->read_mem(addr) |
           (memory_bank_controller->read_mem(addr + 1) << 8);
}
