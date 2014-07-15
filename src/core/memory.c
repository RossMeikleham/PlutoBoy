/*
 * =====================================================================================
 *
 *       Filename:  memory.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/03/14 19:54:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "memory.h"
#include "memory_layout.h"
#include "romInfo.h"
#include "IO.h"
#include <stdio.h>

static uint8_t mem[WORD_MAX - 0x100];

static int rom_banks = 0; //
static int mbc_mode = 0; //memory bank mode

static uint8_t RAM_banks[4][0x2000];
static uint8_t *current_RAM_bank;

// Get the memory bank mode of ROM loaded into memory
static void setup_MBC_mode() {
    uint8_t type = mem[CARTRIDGE_TYPE];

    if  (type == 0) {
        mbc_mode = MBC0;
    } else if (type >=1 || type <= 3) {
        mbc_mode = MBC1;
    } else if (type == 5 || type == 6) {
        mbc_mode = MBC2;
    } else {
        fprintf(stderr, "unknown rom memory type : %u\n",type);
    }
}

static void setup_RAM_banks() {
    // Only MBC1 has RAM banks
    if (mbc_mode == MBC1) {
        
    }

}
/* After initial 32kb of rom read into memory
 * mmu setup should occur */
void setup_mmu() {

    setup_MBC_mode();  
    setup_RAM_banks();      
    //Only MBC1 has RAM banks
    if (mbc_mode == MBC1) {

    }
}


void set_mem(uint16_t const loc, uint8_t const val) {
    if (loc < 0xFF00) {
        mem[loc] = val;
    }
    /*  Check if mirrored memory being written to */
    if (loc >= ECHO_RAM_START && loc <= ECHO_RAM_END) {
        mem[loc-0x2000] = val;
    } else if (loc >= ECHO_RAM_START-0x2000 && loc <= ECHO_RAM_END-0x2000) {
        mem[loc+0x2000] = val;
    }

    /*  IO being written to */
    if (loc >= 0xFF00) {
        io_set_mem(GLOBAL_TO_IO_ADDR(loc), val);
    }
}

uint8_t get_mem(uint16_t const loc) {
    if (loc < 0xFF00) {
        return mem[loc];
    } else {
        return io_get_mem(GLOBAL_TO_IO_ADDR(loc));
    }
}


void set_mem_16(uint16_t const loc, uint16_t const val) {
    set_mem(loc + 1, val >> 8);
    set_mem(loc, val & 0xFF);
}


uint16_t get_mem_16(uint16_t const loc) {
    return (get_mem(loc + 1) << 8) |
            get_mem(loc);
}


// Applies function to 8 bit value at the specified address
void mem_op(uint16_t addr, void (mem_op_fn)(uint8_t *)) {
    uint8_t temp = get_mem(addr); 
    mem_op_fn(&temp); /*  perform modifying op on temp */
    set_mem(addr, temp); /*  write new value temp back */
}



