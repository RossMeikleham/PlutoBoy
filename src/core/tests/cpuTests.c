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
    mu_assert_uint_eq(reg.AF, (reg.A << 8) | reg.F);
    
    reg.B = 0x2;
    reg.C = 0x4;
    mu_assert_uint_eq(reg.BC, (reg.B << 8) | reg.C);
    
    reg.D = 0x20;
    reg.E = 0x45;
    mu_assert_uint_eq(reg.DE, (reg.D << 8) | reg.E);
    
    reg.H = 0x30;
    reg.L = 0x5;
    mu_assert_uint_eq(reg.HL, (reg.H << 8) | reg.L); 
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
    mu_assert_uint_eq(reg.B, reg.C);
}

/*  Test loading reg value into register C */
MU_TEST(test_LD_C) {
    reg.E = 0x56;
    LD_C_E();
    mu_assert_uint_eq(reg.C, reg.E);
}
/*  Test loading reg value into register D */
MU_TEST(test_LD_D) {
    reg.A = 0x87;
    LD_D_A();
    mu_assert_uint_eq(reg.D, reg.A);
}

/*  Test loading reg value into register E */
MU_TEST(test_LD_E) {
    reg.H = 0x99;
    LD_B_H();
    mu_assert_uint_eq(reg.B, reg.H);
}
/*  Test loading reg value into register H */
MU_TEST(test_LD_H) {
    reg.L = 0xB2;
    LD_H_L();
    mu_assert_uint_eq(reg.H, reg.L);
}

/*  Test loading reg value into register L */
MU_TEST(test_LD_L) {
    reg.D = 0xAF;
    LD_L_D();
    mu_assert_uint_eq(reg.L, reg.D);
}


/*  Test loading from memory into reg */
MU_TEST(test_LD_reg_mem) {
    uint16_t mem_loc = 0x100;
    uint8_t val = 0xFF;

    reg.HL = mem_loc;
    set_mem(mem_loc, val);
    LD_H_memHL();
    
    mu_assert_uint_eq(reg.H, val);
}

/*  Test loading from reg to mem */
MU_TEST(test_LD_mem_reg) {
    uint16_t mem_loc = 0xFF10;
    uint8_t val = 0x56;

    reg.HL = mem_loc;
    reg.A = val;
    LD_memHL_A();

    mu_assert_uint_eq(get_mem(reg.HL), val);
}

MU_TEST_SUITE(cpu_instructions) {
    MU_SUITE_CONFIGURE(&setup, &teardown);

    MU_RUN_TEST(test_combined_reg);
    MU_RUN_TEST(test_LD_8_IM);
    MU_RUN_TEST(test_LD_A);
    MU_RUN_TEST(test_LD_B);
    MU_RUN_TEST(test_LD_C);
    MU_RUN_TEST(test_LD_D);
    MU_RUN_TEST(test_LD_E);
    MU_RUN_TEST(test_LD_H);
    MU_RUN_TEST(test_LD_L);

    MU_RUN_TEST(test_LD_reg_mem);
    MU_RUN_TEST(test_LD_mem_reg);
}



int main () {
    MU_RUN_SUITE(cpu_instructions);
    MU_REPORT();
    return 0;
}
