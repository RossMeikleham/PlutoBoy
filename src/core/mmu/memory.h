#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

extern bool bg_palette_dirty;
extern bool sprite_palette_dirty;

// 1 if gameboy is booting up, 0 otherwise
extern int is_booting; 

extern uint8_t *io_mem;
extern uint8_t *oam_mem_ptr;

/* Read from OAM given OAM address 0 - A0
 * Returns 0x0 if address > 0xA0 */
static inline uint8_t oam_get_mem(uint8_t addr) {
    //Check not unusable RAM (i.e. not 0xFEA0 - 0xFEFF)
    return (addr < 0xA0) ? oam_mem_ptr[addr] : 0;
}

/* Directly inject a value into IO memory without performing
 * any checks or operations on the data. Should be used by
 * controllers that have direct access to modifying this memory
 * and not the CPU. */
void set_mem_override(uint16_t loc, uint8_t val); 

// Read a value from a VRAM bank

uint8_t get_current_vram(uint16_t addr);

uint8_t get_vram(uint16_t addr, int bank);

uint8_t get_vram0(uint16_t addr);

uint8_t get_vram1(uint16_t addr);

uint8_t oam_get_mem(uint8_t addr);

void io_write_mem(uint8_t addr, uint8_t val);

// Read contents from given 16 bit memory address
uint8_t get_mem(uint16_t addr);

/*  Write an 8 bit value to the given 16 bit address */
void set_mem(uint16_t addr, uint8_t const val);

/* Write 16bit value starting at the given memory address 
 * into memory.  Written in little-endian byte order */
void set_mem_16(uint16_t const loc, uint16_t const val);


/* Read contents of 2 memory locations starting at the
 * given address. Returned as little-endian byte order 16 bit value */
uint16_t get_mem_16(uint16_t const loc); 
    
/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(char const * filename, uint8_t header[0x50], int const dmg_mode);

// deallocate all allocated memory
void teardown_memory();

// read a value from gameboy color background palette RAM
uint8_t read_bg_color_palette(int addr);

uint8_t *get_bg_palette();

uint8_t *get_sprite_palette();

// Read a vlue from gameboy color sprite palette RAM
uint8_t read_sprite_color_palette(int addr);

/* Given a memory location, buffer, and size of the cache, attempts to load save data from cache into the buffer.
 * Returns the size of the cache if successful, returns 0 if unsuccessful.
 * Buffer should be at minimum of size "MAX_SRAMS_SIZE" */
unsigned long load_SRAM_cached(unsigned char *cache_ptr, unsigned char *data, unsigned long size);

/* Given memory location, save data and the size of save data, attempts to
 * save the data to a cache in ram. Used to improve 3DS emulation speed 
 * and lower the amount of taxing I/O writes. Returns 1 if successful, 0 otherwise */
int save_SRAM_cached(unsigned char *cache_ptr, unsigned char *data, unsigned long size);


int interrupt_about_to_raise();

#endif
