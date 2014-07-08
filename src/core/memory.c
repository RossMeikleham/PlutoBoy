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

static uint8_t mem[WORD_MAX];

void set_mem(uint16_t const loc, uint8_t const val) {
    mem[loc] = val;
    /*  Check if mirrored memory being written to */
    if (loc >= ECHO_RAM_START && loc <= ECHO_RAM_END) {
        mem[loc-0x2000] = val;
    } else if (loc >= ECHO_RAM_START-0x2000 && loc <= ECHO_RAM_END-0x2000) {
        mem[loc+0x2000] = val;
    }
}

uint8_t get_mem(uint16_t const loc) {
    return mem[loc];
}


void set_mem_16(uint16_t const loc, uint16_t const val) {
    set_mem(loc + 1, val >> 8);
    set_mem(mem_loc, value & 0xFF);
}


uint16_t get_mem_16(uint16_t const loc) {
    return (get_mem(mem_loc + 1) << 8) |
            get_mem(mem_loc);
}


