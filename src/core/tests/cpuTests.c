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
#include "minunit/minunit.h"
#include <stdio.h>


/*  Reset CPU and Memory */
void setup() {
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

void teardown() {
    //Nothing
}

/*  Test loading 8 bit immediate values */
 MU_TEST(test_LD_8_IM) {
    
    reg.PC= 1;
    set_mem(reg.PC - 1, 0xFF);
    LD_C_IM();
    mu_check((reg.C == 0xFF));
}

MU_TEST_SUITE(cpu_instructions) {
    MU_SUITE_CONFIGURE(&setup, &teardown);

    MU_RUN_TEST(test_LD_8_IM);
}



int main () {
    MU_RUN_SUITE(cpu_instructions);
    MU_REPORT();
    return 0;
}
