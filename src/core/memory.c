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
#include "IO.h"

static uint8_t mem[WORD_MAX - 0x100];

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


