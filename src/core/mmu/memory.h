#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>
#include <stdlib.h>
#include "mbc.h"
#include "../sound.h"
#include "../sprite_priorities.h"
#include "../rom_info.h"
#include "../memory_layout.h"

uint8_t mem[0xFF00 + 1 - 0x100];
uint8_t io_mem[0x100];
uint8_t oam_mem[0xA0];

void unload_boot_rom();

/* Directly inject a value into IO memory without performing
 * any checks or operations on the data. Should be used by
 * controllers that have direct access to modifying this memory
 * and not the CPU. */
void io_write_override(uint8_t loc, uint8_t val);


void set_mem_override(uint16_t loc, uint8_t val); 

/* Write 16bit value starting at the given memory address 
 * into memory.  Written in little-endian byte order */
void set_mem_16(uint16_t loc, uint16_t val);


/* Read contents of 2 memory locations starting at the
 * given address. Returned as little-endian byte order 16 bit value */
uint16_t get_mem_16(uint16_t loc); 
    
/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(char const * filename, unsigned char const *file_data, size_t size);

/* Transfer 160 bytes to sprite memory starting from
 * address XX00 */
void dma_transfer(uint8_t val);

void joypad_write(uint8_t joypad_state);

/* Write to IO memory given address 0 - 0xFF */
inline void io_write_mem(uint8_t addr, uint8_t val) {

  
    io_mem[addr] = val;
    uint16_t global_addr = addr + 0xFF00;
    if (global_addr >= 0xFF10 && global_addr <= 0xFF3F) {
        write_apu(global_addr, val);
        return;
    }
    switch (global_addr) {
       
        case P1_REG  : joypad_write(val); break;

        /*  Attempting to set DIV reg resets it to 0 */
        case DIV_REG  : io_mem[addr] = 0 ;break; //io_mem[addr] = 0; break;
        /*  Attempting to set LY reg resets it to 0  */
        case LY_REG   : io_mem[addr] = 0; break;
        /*  Perform direct memory transfer  */
        case DMA_REG  : dma_transfer(val); break;
    }
}

/* Write to OAM given OAM address 0x0 - 0xA0
 * Does nothing if address > 0xA0 */
inline void oam_set_mem(uint8_t addr, uint8_t val) {
    
    // Check not unusable RAM (i.e. not 0xFEA0 - 0xFEFF)
    if (addr < 0xA0) {
        oam_mem[addr] = val;
        /* If Object X position is written to, reorganise
         * sprite priorities for rendering */
        if((addr - 1) % 4 == 0) {
            update_sprite_prios(addr/4 ,val);
        }
    }
}


/* Read from OAM given OAM address 0 - A0
 * Returns 0x0 if addres > 0xA0 */
inline uint8_t oam_get_mem(uint8_t addr) {
    //Check not unusable RAM (i.e. not 0xFEA0 - 0xFEFF)
    return (addr < 0xA0) ? oam_mem[addr] : 0;
}


/*  Write an 8 bit value to the given 16 bit address */
inline void set_mem(uint16_t const addr, uint8_t const val) {
    
    //Check if memory bank controller chip is being accessed 
    if (addr < 0x8000 || ((uint16_t)(addr - 0xA000) < 0x2000)) {
        write_MBC(addr, val); 
        return;
    }
    // Write to "ordinary" memory (0x8000 - 0xFDFF)
    if (addr < 0xFE00) {
        mem[addr] = val;
        //  Check if mirrored memory being written to

        // 0xE000 - 0xFDFF, repeat to 0xC000 - 0xDDFF
        if ((uint16_t)(addr - ECHO_RAM_START) < 0x1DFF) { 
            mem[addr - 0x2000] = val;
        // 0xC000 - 0xDDFF, repeat to 0xE000 - 0xFDFF
        } else if ((uint16_t)(addr - ECHO_RAM_START - 0x2000) < 0x1DFF) {
            mem[addr + 0x2000] = val;
        }
        return;
    }
    // Write to Object Attribute Table (0xFE00 - 0xFEFF)
    if ((uint16_t)(addr - 0xFE00) < 0x100) {
        oam_set_mem(addr - 0xFE00, val);
        return;
    }
    /*  IO being written to */
    if (addr >= 0xFF00) {
        // Boot ROM finished, reload start of cartridge
        if (addr == 0xFF50 && val == 1) {
            unload_boot_rom();
        }
        io_write_mem(addr - 0xFF00, val);
    }
}


// Read contents from given 16 bit memory address
inline uint8_t get_mem(uint16_t const addr) {
    
    // Check if reading from Memory Bank Controller
    if (addr < 0x8000 || ((uint16_t)(addr - 0xA000) < 0x2000)) {
        return read_MBC(addr);   
    }
    // Read from "ordinary" memory (0x8000 - 0xFDFF)
    if (addr < 0xFE00) {
        return mem[addr];
    }
    // Read from Object Attribute Table (0xFE00 - 0xFEFF) 
    if ((uint16_t)(addr - 0xFE00) < 0x100) {
        return oam_get_mem(addr - 0xFE00);;
    }
    // Read from IO mem
    if (addr >= 0xFF10 && addr <= 0xFF3F) {
        return read_apu(addr);
    }
    return io_mem[addr - 0xFF00];

}





#endif
