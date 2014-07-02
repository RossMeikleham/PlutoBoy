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


/*  Test combined registers */
MU_TEST(test_combined_reg) {

    reg.A = 0x1;
    reg.F = 0x2;
#include <stdio.h>
    printf("stuff %u\n",reg.AF);
    mu_assert_uint_eq(reg.AF, (reg.A << 4) | reg.F);
    
    reg.B = 0x2;
    reg.C = 0x4;
    mu_assert_uint_eq(reg.BC, (reg.B << 4) | reg.C);
    
    reg.D = 0x20;
    reg.E = 0x45;
    mu_assert_uint_eq(reg.DE, (reg.D << 4) | reg.E);
    
    reg.H = 0x30;
    reg.L = 0x5;
    mu_assert_uint_eq(reg.HL, (reg.H << 4) | reg.L); 
}

/*  Test loading 8 bit immediate values */
MU_TEST(test_LD_8_IM) {
    
    reg.PC= 1;
    uint8_t value = 0xFF;
    set_mem(reg.PC - 1, value);
    LD_C_IM();
    mu_assert_uint_eq(reg.C, value);
}


/*  Test loading reg value into register A */
MU_TEST(test_LD_A) {
    reg.B = 0x1;
    LD_A_B();
    mu_assert_uint_eq(reg.A, reg.B);
}

/*  Test loading reg value into register B */
MU_TEST(test_LD_B) {
    reg.C = 0x35;
    LD_B_C();
    mu_assert_uint_eq(reg.C, reg.B);
}



MU_TEST_SUITE(cpu_instructions) {
    MU_SUITE_CONFIGURE(&setup, &teardown);

    MU_RUN_TEST(test_combined_reg);
    MU_RUN_TEST(test_LD_8_IM);
    MU_RUN_TEST(test_LD_A);
    MU_RUN_TEST(test_LD_B);
}



int main () {
    MU_RUN_SUITE(cpu_instructions);
    MU_REPORT();
    return 0;
}
