/*
 * =====================================================================================
 *
 *       Filename:  cpuTests.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  22/06/14 09:25:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "../cpu.c"
#include "../memory.h"
#include "minunit.h"
#include <stdio.h>


void before() {
    reg.AF = 0;
    reg.BC = 0;
    reg.DE = 0;
    reg.HL = 0;
    reg.PC = 0;
    reg.SP = 0;

    for (int i = 0; i < 0xFFFF; i++) {
        set_mem(i, 0x0);
        
    }
}

/*  Test loading 8 bit immediate values */
const char *test_LD_8_IM() {
    
    reg.PC= 1;
    set_mem(reg.PC - 1, 0xFF);
    LD_C_IM();
    mu_assert((reg.C == 0xFF) , "Immediate value was incorrect");
    return NULL;
}


const char *all_tests() {
    SET_BEFORE(before);
    mu_suite_start();

    mu_run_test(test_LD_8_IM);

    return NULL;
}

RUN_TESTS(all_tests);

