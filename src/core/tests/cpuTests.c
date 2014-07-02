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

/*  Test loading immediate value into memory */
MU_TEST(test_LD_HL_n) {
    uint8_t val = 0x39;
    uint16_t mem_loc = 0x1111;

    reg.HL = mem_loc;
    reg.PC = 2;
    set_mem(reg.PC - 1, val);
    LD_HL_n();

    mu_assert_uint_eq(get_mem(reg.HL), val);

}

/* Test loading val in memory at reg BC into reg A */
MU_TEST(test_LD_A_memBC) {
    uint8_t val = 0x55;
    uint16_t mem_loc = 0x9001;

    reg.BC = mem_loc;
    set_mem(mem_loc, val);
    LD_A_memBC();

    mu_assert_uint_eq(reg.A, val);
}


/* Test loading val in memory at reg DE into reg A */
MU_TEST(test_LD_A_memDE) {
    uint8_t val = 0x99;
    uint16_t mem_loc = 0x2;

    reg.DE = mem_loc;
    set_mem(mem_loc, val);
    LD_A_memDE();

    mu_assert_uint_eq(reg.A, val);
}


/* Test loading val in memory at immediate 16 bytes into reg A */
MU_TEST(test_LD_A_memnn) {
    uint8_t val = 0x77;
    uint16_t mem_loc = 0x2014;

    reg.PC = 10;
    set_mem(mem_loc, val);
    set_mem(reg.PC - 2, (mem_loc & 0xFF));
    set_mem(reg.PC - 1, (mem_loc >> 8));
    LD_A_memnn();

    mu_assert_uint_eq(reg.A, val);
}

/* Test loading register A into memory location at reg BC */
MU_TEST(test_LD_memBC_A) {
    uint8_t val = 0x82;
    uint16_t mem_loc = 0x6666;
    
    reg.BC = mem_loc;
    reg.A = val;
    LD_memBC_A();

    mu_assert_uint_eq(get_mem(reg.BC), reg.A);
}

/* Test loading register A into memory location at reg DE */
MU_TEST(test_LD_memDE_A) {
    uint8_t val = 0x91;
    uint16_t mem_loc = 0x7701;
    
    reg.DE = mem_loc;
    reg.A = val;
    LD_memDE_A();

    mu_assert_uint_eq(get_mem(reg.DE), reg.A);
}


/*  Test loading register A into memory location at immediate 16 bytes */
MU_TEST(test_LD_memnn_A) {
    uint8_t val = 0xCA;
    uint16_t mem_loc = 0x256;

    reg.PC = 102;
    reg.A = val;

    set_mem(reg.PC - 2, (mem_loc & 0xFF));
    set_mem(reg.PC - 1, (mem_loc >> 8));
    LD_memnn_A();

    mu_assert_uint_eq(get_mem(mem_loc), reg.A);
}


/*  Test loading val at mem HL into reg A and decrementing HL */
MU_TEST(test_LDD_A_HL) {
    uint8_t val = 0x73;
    uint16_t mem_loc = 0xDEF0;

    reg.HL = mem_loc;
    set_mem(reg.HL, val);
    LDD_A_HL();

    mu_assert_uint_eq(reg.A, val);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc -1));
}

/*  Test when HL is 0 it negative overflows to 0xFFFF after
 *  LDD_A_HL instruction */
MU_TEST(test_LDD_A_HL_underflow) {
    uint8_t val = 0x98;
    uint16_t mem_loc = 0x0000;

    reg.HL = mem_loc;
    set_mem(reg.HL, val);
    LDD_A_HL();

    mu_assert_uint_eq(reg.A, val);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc - 1));
}

/*  Test loading val at reg A into mem HL and decrementing HL */
MU_TEST(test_LDD_HL_A) {
    uint8_t val = 0x3F;
    uint16_t mem_loc = 0x3456;

    reg.A = val;
    reg.HL = mem_loc;
    LDD_HL_A();

    mu_assert_uint_eq(get_mem(mem_loc), reg.A);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc - 1));
}

/*  Test when HL is 0 it negative overlows to 0xFFFF after
 *  LDD_HL_A instruction */
MU_TEST(test_LDD_HL_A_underflow) {
    uint8_t val = 0x5A;
    uint16_t mem_loc = 0x0000;

    reg.A = val;
    reg.HL = mem_loc;
    LDD_HL_A();

    mu_assert_uint_eq(get_mem(mem_loc), reg.A);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc - 1));
}

/*  Test loading val at mem HL into reg A and incrementing HL */
MU_TEST(test_LDI_A_HL) {
    uint8_t val = 0x55;
    uint16_t mem_loc = 0xEF03;

    reg.HL = mem_loc;
    set_mem(reg.HL, val);
    LDI_A_HL();

    mu_assert_uint_eq(reg.A, val);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc + 1));
}

/*  Test when HL is 0xFFFF it overflows to 0x after0
 *  LDI_A_HL instruction */
MU_TEST(test_LDI_A_HL_overflow) {
    uint8_t val = 0x73;
    uint16_t mem_loc = 0xFFFF;

    reg.HL = mem_loc;
    set_mem(reg.HL, val);
    LDI_A_HL();

    mu_assert_uint_eq(reg.A, val);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc + 1));
}

/*  Test loading val at reg A into mem HL and incrementing HL */
MU_TEST(test_LDI_HL_A) {
    uint8_t val = 0x69;
    uint16_t mem_loc = 0xFEED;

    reg.A = val;
    reg.HL = mem_loc;
    LDI_HL_A();

    mu_assert_uint_eq(get_mem(mem_loc), reg.A);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc + 1));
}

/*  Test when HL is 0xFFFF it overlows to 0x0 after
 *  LDI_HL_A instruction */
MU_TEST(test_LDI_HL_A_overflow) {
    uint8_t val = 0x99;
    uint16_t mem_loc = 0xFFFF;

    reg.A = val;
    reg.HL = mem_loc;
    LDI_HL_A();

    mu_assert_uint_eq(get_mem(mem_loc), reg.A);
    mu_assert_uint_eq(reg.HL, (uint16_t)(mem_loc + 1));
}



/*  Test loading val at reg A into mem $FF00 + immediate 8 bit val */
MU_TEST(test_LDH_n_A) {
    uint8_t val = 0x78;
    uint8_t im_val = 0xFF;

    reg.A = val;
    reg.PC = 0x207;
    set_mem(reg.PC - 1, im_val);
    LDH_n_A();

    mu_assert_uint_eq(get_mem(0xFF00 + im_val), reg.A);
}

/*  Test loading val at mem $FF00 + immediate 8 bit val into reg A */
MU_TEST(test_LDH_A_n) {
    uint8_t val = 0x66;
    uint8_t im_val = 0x01;

    reg.PC = 0x207;
    set_mem(reg.PC - 1, im_val);
    set_mem(0xFF00 + im_val, val);

    LDH_A_n();

    mu_assert_uint_eq(val, reg.A);
}

/*  Test loading val at mem $FF00 + val at reg C into reg A */
MU_TEST(test_LDH_A_C) {
    uint8_t val = 0x16;
    uint8_t im_val = 0x0;

    reg.C = im_val;
    set_mem(0xFF00 + reg.C, val);
    LDH_A_C();

    mu_assert_uint_eq(val, reg.A);
}

/*  Test loading val at reg A into mem $FF00 + val at reg C */
MU_TEST(test_LDH_C_A) {
    uint8_t val = 0x37;
    uint8_t im_val = 0x92;
    
    reg.A = val;
    reg.C = im_val;
    LDH_C_A();

    mu_assert_uint_eq(val, get_mem(0xFF00 + reg.C));
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
    MU_RUN_TEST(test_LD_HL_n);

    MU_RUN_TEST(test_LD_A_memBC);
    MU_RUN_TEST(test_LD_A_memDE);
    MU_RUN_TEST(test_LD_A_memnn);

    MU_RUN_TEST(test_LD_memBC_A);
    MU_RUN_TEST(test_LD_memDE_A);
    MU_RUN_TEST(test_LD_memnn_A);

    MU_RUN_TEST(test_LDD_A_HL);
    MU_RUN_TEST(test_LDD_A_HL_underflow);
    MU_RUN_TEST(test_LDD_HL_A);
    MU_RUN_TEST(test_LDD_HL_A_underflow);

    MU_RUN_TEST(test_LDI_A_HL);
    MU_RUN_TEST(test_LDI_A_HL_overflow);
    MU_RUN_TEST(test_LDI_HL_A);
    MU_RUN_TEST(test_LDI_HL_A_overflow);

    MU_RUN_TEST(test_LDH_n_A);
    MU_RUN_TEST(test_LDH_A_n);
    MU_RUN_TEST(test_LDH_A_C);
    MU_RUN_TEST(test_LDH_C_A);
}



int main () {
    MU_RUN_SUITE(cpu_instructions);
    MU_REPORT();
    return 0;
}
