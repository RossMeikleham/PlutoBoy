/* Modified Z80 GameBoy CPU*/
/* Ross Meikleham */
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "cpu.h"
#include <string.h>
#include <stdlib.h>
#include "disasm.h"
#include "registers.h"
#include "timers.h"

#define IMMEDIATE_8_BIT get_mem(reg.PC-1)
#define IMMEDIATE_16_BIT (get_mem((reg.PC)-1)<< 8) | get_mem((reg.PC-2))
#define SIGNED_IM_8_BIT ((IMMEDIATE_8_BIT & 127) - (IMMEDIATE_8_BIT & 128))
#define SIGNED_IM_16_BIT ((IMMEDIATE_16_BIT & 0xFFFE) - (IMMEDIATE_16_BIT & 0xFFFF))


static int halt = 0;
static int stop = 0;
static int interrupts_enabled = 1;
static int interrupts_enabled_timer = 0;
static uint8_t opcode;

static int timer_cycles_passed = 0;
//TODO preprocessor check of endian-ness of processor
//and switch hi/lo registers for big endian processors
union {
  struct{uint16_t AF,BC,DE,HL,SP,PC;};
  
  struct{uint8_t F,A,C,B,E,D,L,H,SPlo,SPhi,PClo,PChi;};
  struct{uint8_t UNUSED_FLAG4:1, UNUSED_FLAG3:1, UNUSED_FLAG2:1, 
  UNUSED_FLAG:1, C_FLAG:1, H_FLAG:1, N_FLAG:1, Z_FLAG:1;};

} static reg;


// Pointer to function which performs an operation
// based on an opcode
typedef void (*Operation)(void); 


typedef struct {

    int cycles; /*  Number of machine cycles per instruction */
    Operation const operation; /* operation which performs the instruction */
    
} Instruction;

extern Instruction ins[UINT8_MAX+1];

/*  Information on all processor instructions 
 *  including extended instructions  */
typedef struct {

    Instruction * instruction_set; 
    int const * words; /*  No of words per instruction */
    Instruction const * const ext_instruction_set; /* extended 0xCB instructions */


} Instructions;



/* ***************Opcodes ********************* */


void invalid_op(){
    fprintf(stderr, "Error, unknown opcode: %x\n", opcode);
    exit(1);
}


/************* 8 Bit Load Operations ********************/


/* Load 8 bit immediate value into specified location */
void LD_8_IM(uint8_t *loc) { *loc = IMMEDIATE_8_BIT;}

void LD_A_IM(){LD_8_IM(&reg.A);}
void LD_B_IM(){LD_8_IM(&reg.B);}
void LD_C_IM(){LD_8_IM(&reg.C);}
void LD_D_IM(){LD_8_IM(&reg.D);}
void LD_E_IM(){LD_8_IM(&reg.E);}
void LD_H_IM(){LD_8_IM(&reg.H);}
void LD_L_IM(){LD_8_IM(&reg.L);}

/* Load register value into reg A */
void LD_A_A() {reg.A = reg.A;}
void LD_A_B() {reg.A = reg.B;}
void LD_A_C() {reg.A = reg.C;}
void LD_A_D() {reg.A = reg.D;}
void LD_A_E() {reg.A = reg.E;}
void LD_A_H() {reg.A = reg.H;}
void LD_A_L() {reg.A = reg.L;}

/*  Load register value into reg B */
void LD_B_A() {reg.B = reg.A;}
void LD_B_B() {reg.B = reg.B;}
void LD_B_C() {reg.B = reg.C;}
void LD_B_D() {reg.B = reg.D;}
void LD_B_E() {reg.B = reg.E;}
void LD_B_H() {reg.B = reg.H;}
void LD_B_L() {reg.B = reg.L;}

/*  Load register value into reg C */
void LD_C_A() {reg.C = reg.A;}
void LD_C_B() {reg.C = reg.B;}
void LD_C_C() {reg.C = reg.C;}
void LD_C_D() {reg.C = reg.D;}
void LD_C_E() {reg.C = reg.E;}
void LD_C_H() {reg.C = reg.H;}
void LD_C_L() {reg.C = reg.L;}


/* Load register value into reg D */
void LD_D_A() {reg.D = reg.A;}
void LD_D_B() {reg.D = reg.B;}
void LD_D_C() {reg.D = reg.C;}
void LD_D_D() {reg.D = reg.D;}
void LD_D_E() {reg.D = reg.E;}
void LD_D_H() {reg.D = reg.H;}
void LD_D_L() {reg.D = reg.L;}


/*  Load register value into reg E */
void LD_E_A() {reg.E = reg.A;}
void LD_E_B() {reg.E = reg.B;}
void LD_E_C() {reg.E = reg.C;}
void LD_E_D() {reg.E = reg.D;}
void LD_E_E() {reg.E = reg.E;}
void LD_E_H() {reg.E = reg.H;}
void LD_E_L() {reg.E = reg.L;}


/* Load register value into reg H */
void LD_H_A() {reg.H = reg.A;}
void LD_H_B() {reg.H = reg.B;}
void LD_H_C() {reg.H = reg.C;}
void LD_H_D() {reg.H = reg.D;}
void LD_H_E() {reg.H = reg.E;}
void LD_H_H() {reg.H = reg.H;}
void LD_H_L() {reg.H = reg.L;}


/* Load register value into reg L */
void LD_L_A() {reg.L = reg.A;}
void LD_L_B() {reg.L = reg.B;}
void LD_L_C() {reg.L = reg.C;}
void LD_L_D() {reg.L = reg.D;}
void LD_L_E() {reg.L = reg.E;}
void LD_L_H() {reg.L = reg.H;}
void LD_L_L() {reg.L = reg.L;}



/* Load value into register from address at reg HL */
void LD_A_memHL() {reg.A = get_mem(reg.HL);}
void LD_B_memHL() {reg.B = get_mem(reg.HL);}
void LD_C_memHL() {reg.C = get_mem(reg.HL);}
void LD_D_memHL() {reg.D = get_mem(reg.HL);}
void LD_E_memHL() {reg.E = get_mem(reg.HL);}
void LD_H_memHL() {reg.H = get_mem(reg.HL);}
void LD_L_memHL() {reg.L = get_mem(reg.HL);}



/* Load value from register r to mem location HL */
void LD_memHL_A() {set_mem(reg.HL, reg.A);}
void LD_memHL_B() {set_mem(reg.HL, reg.B);}
void LD_memHL_C() {set_mem(reg.HL, reg.C);}
void LD_memHL_D() {set_mem(reg.HL, reg.D);}
void LD_memHL_E() {set_mem(reg.HL, reg.E);}
void LD_memHL_H() {set_mem(reg.HL, reg.H);}
void LD_memHL_L() {set_mem(reg.HL, reg.L);}


/* Load immediate value into memory location HL */
void LD_memHL_n() {update_timers(4); timer_cycles_passed = 4; set_mem(reg.HL, IMMEDIATE_8_BIT);}

/*  Load value at mem address given by combined registers into A */
void LD_A_memBC() { reg.A = get_mem(reg.BC); }
void LD_A_memDE() { reg.A = get_mem(reg.DE); }

/* Load value at memory address given by immediate 16 bits into A */
void LD_A_memnn() { 
    update_timers(8); 
    reg.A = get_mem(IMMEDIATE_16_BIT);
    timer_cycles_passed = 8;
}

/* Load A into memory address contained at register BC */
void LD_memBC_A() { set_mem(reg.BC, reg.A); }

/* Load A into memory address contained at registers DE  */
void LD_memDE_A() { set_mem(reg.DE, reg.A); }

/*  Load A into memory address given by immediate 16 bits */
void LD_memnn_A() { 
    update_timers(8);
    set_mem(IMMEDIATE_16_BIT, reg.A);
    timer_cycles_passed = 8;
}

/* Put value at address HL into A, then decrement HL */
void LDD_A_HL() { reg.A = get_mem(reg.HL); reg.HL--; }

/* Put A into memory address HL, then decrement HL */
void LDD_HL_A() { set_mem(reg.HL, reg.A); reg.HL--; }

/* Put value at address HL into A, then increment HL */ 
void LDI_A_HL() { reg.A = get_mem(reg.HL); reg.HL++; }

/* Put A into memory address HL then increment HL */
void LDI_HL_A() { set_mem(reg.HL, reg.A); reg.HL++; }

/* Put A into memory address $FF00+n*/
void LDH_n_A() { 
    update_timers(4); 
    set_mem(0xFF00 + IMMEDIATE_8_BIT, reg.A);
    timer_cycles_passed = 4;
}

/* Put memory address $FF00+n into A */
void LDH_A_n() { 
    update_timers(4); 
    reg.A = get_mem(0xFF00 + IMMEDIATE_8_BIT); 
    timer_cycles_passed = 4;
}

/* Put memory address $FF00 + C into A */
void LDH_A_C() {reg.A = get_mem(0xFF00 + reg.C);}

/* Put A into memory address $FF00 + C */
void LDH_C_A() {set_mem(0xFF00 + reg.C, reg.A);}



/******************* 16 bit load operations ****************/


/*  Load 16 bit immediate value into combined reg */
void LD_16_IM(uint16_t *r){*r = IMMEDIATE_16_BIT;}

void LD_BC_IM() {LD_16_IM(&reg.BC);}
void LD_DE_IM() {LD_16_IM(&reg.DE);}
void LD_HL_IM() {LD_16_IM(&reg.HL);}
void LD_SP_IM() {LD_16_IM(&reg.SP);}


/*  Load HL into stack pointer */
void LD_SP_HL() {reg.SP = reg.HL;}

/*  Place SP + Immediate 8 bit into HL */
void LD_HL_SP_n() {

    reg.Z_FLAG = reg.N_FLAG =  0;
    int8_t s8 = SIGNED_IM_8_BIT;
    reg.HL = reg.SP + s8;

    //TODO find out why this works :|
    uint16_t temp = reg.SP ^ s8 ^ reg.HL;
    reg.C_FLAG = !!(temp & 0x100);
    reg.H_FLAG = !!(temp & 0x10);
}


/* Place SP into memory at immediate address nn */
void LD_nn_SP() {set_mem_16(IMMEDIATE_16_BIT, reg.SP); }


/* Push register pair onto the stack */
void PUSH(uint16_t r) {reg.SP-=2; set_mem_16(reg.SP, r);}

void PUSH_AF() {PUSH(reg.AF);}
void PUSH_BC() {PUSH(reg.BC);}
void PUSH_DE() {PUSH(reg.DE);}
void PUSH_HL() {PUSH(reg.HL);}


/* Pop value from stack into register pair*/
void POP(uint16_t *r) {*r = get_mem_16(reg.SP); reg.SP+=2;}

void POP_AF() {
    POP(&(reg.AF));
    reg.F &= 0xF0; //Lower nibble of F should always be 0
}
	



void POP_BC() {POP(&(reg.BC));}
void POP_DE() {POP(&(reg.DE));}
void POP_HL() {POP(&(reg.HL));}


/**********************  8 bit ALU *****************/

/* Reset flags, add value2 to value1, set appropriate flags */
static inline uint8_t ADD_8(uint8_t val1, uint8_t val2) 
{ 
    reg.N_FLAG = 0;
    reg.H_FLAG = ((val1 & 0xF) + (val2 & 0xF)) > 0xF ? 1 : 0; 
    val1 += val2; 
    reg.C_FLAG = val1 < val2 ? 1 : 0;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
    return val1;
}


void ADD_A_A(){reg.A = ADD_8(reg.A, reg.A);}
void ADD_A_B(){reg.A = ADD_8(reg.A, reg.B);}
void ADD_A_C(){reg.A = ADD_8(reg.A, reg.C);}
void ADD_A_D(){reg.A = ADD_8(reg.A, reg.D);} 
void ADD_A_E(){reg.A = ADD_8(reg.A, reg.E);} 
void ADD_A_H(){reg.A = ADD_8(reg.A, reg.H);} 
void ADD_A_L(){reg.A = ADD_8(reg.A, reg.L);} 
void ADD_A_memHL(){reg.A = ADD_8(reg.A, get_mem(reg.HL));}
void ADD_A_Im8(){reg.A = ADD_8(reg.A, IMMEDIATE_8_BIT);}    

static inline uint8_t ADC_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = (val1 & 0xF) + (val2 & 0xF) + reg.C_FLAG > 0xF ? 1 : 0;
    val1 += val2 + reg.C_FLAG;
    reg.C_FLAG = ( val1 < val2 || (reg.C_FLAG == 1 && val1 == val2) ) ? 1 : 0;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
    return val1;
    
}

void ADC_A_A(){reg.A = ADC_8(reg.A, reg.A);}
void ADC_A_B(){reg.A = ADC_8(reg.A, reg.B);}
void ADC_A_C(){reg.A = ADC_8(reg.A, reg.C);}
void ADC_A_D(){reg.A = ADC_8(reg.A, reg.D);} 
void ADC_A_E(){reg.A = ADC_8(reg.A, reg.E);} 
void ADC_A_H(){reg.A = ADC_8(reg.A, reg.H);} 
void ADC_A_L(){reg.A = ADC_8(reg.A, reg.L);} 
void ADC_A_memHL(){reg.A = ADC_8(reg.A, get_mem(reg.HL));}
void ADC_A_Im8(){reg.A = ADC_8(reg.A, IMMEDIATE_8_BIT);}    


static inline uint8_t SUB_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 1;
    reg.H_FLAG = (val1 & 0xF) < (val2 & 0xF) ? 1 : 0;
    reg.C_FLAG = val1 < val2 ? 1 : 0;
    reg.Z_FLAG = val1  == val2 ? 1 : 0;
    return val1 - val2;
}

void SUB_A_A(){reg.A = SUB_8(reg.A, reg.A);}
void SUB_A_B(){reg.A = SUB_8(reg.A, reg.B);}
void SUB_A_C(){reg.A = SUB_8(reg.A, reg.C);}
void SUB_A_D(){reg.A = SUB_8(reg.A, reg.D);} 
void SUB_A_E(){reg.A = SUB_8(reg.A, reg.E);} 
void SUB_A_H(){reg.A = SUB_8(reg.A, reg.H);} 
void SUB_A_L(){reg.A = SUB_8(reg.A, reg.L);} 
void SUB_A_memHL(){reg.A = SUB_8(reg.A, get_mem(reg.HL));}
void SUB_A_Im8(){reg.A = SUB_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs SUB carry operation on 2 bytes, returns result and sets flags */
static inline uint8_t SBC_8(uint8_t val1, uint8_t val2)
{
    int16_t result = val1 - val2 - reg.C_FLAG;
    reg.N_FLAG = 1;
    reg.Z_FLAG = (result & 0xFF) == 0;
    reg.H_FLAG = (val1 & 0xF) - (val2 & 0xF) - reg.C_FLAG < 0;
    reg.C_FLAG = result < 0;
    return result & 0xFF;
}

void SBC_A_A(){reg.A = SBC_8(reg.A, reg.A);}
void SBC_A_B(){reg.A = SBC_8(reg.A, reg.B);}
void SBC_A_C(){reg.A = SBC_8(reg.A, reg.C);}
void SBC_A_D(){reg.A = SBC_8(reg.A, reg.D);} 
void SBC_A_E(){reg.A = SBC_8(reg.A, reg.E);} 
void SBC_A_H(){reg.A = SBC_8(reg.A, reg.H);} 
void SBC_A_L(){reg.A = SBC_8(reg.A, reg.L);} 
void SBC_A_memHL(){reg.A = SBC_8(reg.A, get_mem(reg.HL));}
void SBC_A_Im8() { reg.A = SBC_8(reg.A, IMMEDIATE_8_BIT);}  





/* Performs AND operation on 2 bytes, returns result and sets flags */
static inline uint8_t AND_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = 1;
    reg.C_FLAG = 0;
    val1 = val1 & val2;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
    return val1;
}

void AND_A_A(){reg.A = AND_8(reg.A, reg.A);}
void AND_A_B(){reg.A = AND_8(reg.A, reg.B);}
void AND_A_C(){reg.A = AND_8(reg.A, reg.C);}
void AND_A_D(){reg.A = AND_8(reg.A, reg.D);} 
void AND_A_E(){reg.A = AND_8(reg.A, reg.E);} 
void AND_A_H(){reg.A = AND_8(reg.A, reg.H);} 
void AND_A_L(){reg.A = AND_8(reg.A, reg.L);} 
void AND_A_memHL(){reg.A = AND_8(reg.A, get_mem(reg.HL));}
void AND_A_Im8(){reg.A = AND_8(reg.A, IMMEDIATE_8_BIT);}  



/*  Performs OR operation on 2 bytes, returns result and sets flags */

static inline uint8_t OR_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = 0;
    reg.C_FLAG = 0;
    val1 = val1 | val2;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
    return val1;
}

void OR_A_A(){reg.A = OR_8(reg.A, reg.A);}
void OR_A_B(){reg.A = OR_8(reg.A, reg.B);}
void OR_A_C(){reg.A = OR_8(reg.A, reg.C);}
void OR_A_D(){reg.A = OR_8(reg.A, reg.D);} 
void OR_A_E(){reg.A = OR_8(reg.A, reg.E);} 
void OR_A_H(){reg.A = OR_8(reg.A, reg.H);} 
void OR_A_L(){reg.A = OR_8(reg.A, reg.L);} 
void OR_A_memHL(){reg.A = OR_8(reg.A, get_mem(reg.HL));}
void OR_A_Im8(){reg.A = OR_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs XOR operation on 2 bytes, returns result and sets flags */
static inline uint8_t XOR_8(uint8_t val1, uint8_t val2) 
{
    reg.N_FLAG = 0;
    reg.H_FLAG = 0;
    reg.C_FLAG = 0;
    val1 = val1 ^ val2;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
    return val1;

}

void XOR_A_A(){reg.A = XOR_8(reg.A, reg.A);}
void XOR_A_B(){reg.A = XOR_8(reg.A, reg.B);}
void XOR_A_C(){reg.A = XOR_8(reg.A, reg.C);}
void XOR_A_D(){reg.A = XOR_8(reg.A, reg.D);} 
void XOR_A_E(){reg.A = XOR_8(reg.A, reg.E);} 
void XOR_A_H(){reg.A = XOR_8(reg.A, reg.H);} 
void XOR_A_L(){reg.A = XOR_8(reg.A, reg.L);} 
void XOR_A_memHL(){reg.A = XOR_8(reg.A, get_mem(reg.HL));}
void XOR_A_Im8(){reg.A = XOR_8(reg.A, IMMEDIATE_8_BIT); }  


/*  Performs Compare operation on 2 bytes, sets flags */
static inline void CP_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 1;
    reg.H_FLAG = (val1 & 0xF) < (val2 & 0xF) ? 1 : 0;
    reg.C_FLAG = val1 < val2 ? 1 : 0;
    reg.Z_FLAG = val1 == val2 ? 1 : 0;
}

void CP_A_A(){ CP_8(reg.A, reg.A);}
void CP_A_B(){ CP_8(reg.A, reg.B);}
void CP_A_C(){ CP_8(reg.A, reg.C);}
void CP_A_D(){ CP_8(reg.A, reg.D);} 
void CP_A_E(){ CP_8(reg.A, reg.E);} 
void CP_A_H(){ CP_8(reg.A, reg.H);} 
void CP_A_L(){CP_8(reg.A, reg.L);} 
void CP_A_memHL(){ CP_8(reg.A, get_mem(reg.HL));}
void CP_A_Im8(){CP_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs Increment operation on register, sets flags */
static inline uint8_t INC_8(uint8_t val)
{
    val++; 
    reg.N_FLAG = 0;
    reg.H_FLAG = (val & 0xF) == 0 ? 1 : 0;
    reg.Z_FLAG = val == 0 ? 1 : 0;
    return val;
}

void INC_A(){reg.A = INC_8(reg.A);}
void INC_B(){reg.B = INC_8(reg.B);}
void INC_C(){reg.C = INC_8(reg.C);}
void INC_D(){reg.D = INC_8(reg.D);} 
void INC_E(){reg.E = INC_8(reg.E);} 
void INC_H(){reg.H = INC_8(reg.H);} 
void INC_L(){reg.L = INC_8(reg.L);} 
void INC_memHL(){ 
    uint8_t inc = INC_8(get_mem(reg.HL));
    update_timers(4); 
    set_mem(reg.HL, inc);
    timer_cycles_passed = 4;
}


/*  Performs Decrement operation on register, sets flags */
static inline uint8_t DEC_8(uint8_t val)
{
    val--;
    reg.N_FLAG = 1;
    reg.H_FLAG = (val & 0xF) == 0xF ? 1 : 0;
    reg.Z_FLAG = !val;
    return val; 
}


void DEC_A(){reg.A = DEC_8(reg.A);}
void DEC_B(){reg.B = DEC_8(reg.B);}
void DEC_C(){reg.C = DEC_8(reg.C);}
void DEC_D(){reg.D = DEC_8(reg.D);} 
void DEC_E(){reg.E = DEC_8(reg.E);} 
void DEC_H(){reg.H = DEC_8(reg.H);} 
void DEC_L(){reg.L = DEC_8(reg.L);} 
void DEC_memHL(){ 
    uint8_t dec = DEC_8(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, dec);
    timer_cycles_passed = 4;
}



/**********************  16 bit ALU *****************/

/*  Performs Add for 2 16bit values, sets flags */
static inline uint16_t ADD_16(uint16_t val1, uint16_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = (val1 & 0x0FFF) + (val2 & 0x0FFF) > 0x0FFF;
    reg.C_FLAG = 0xFFFF - val1 < val2;
    return val1 + val2;
}

void ADD_HL_BC() {reg.HL = ADD_16(reg.HL, reg.BC);}
void ADD_HL_DE() {reg.HL = ADD_16(reg.HL, reg.DE);}
void ADD_HL_HL() {reg.HL = ADD_16(reg.HL, reg.HL);}
void ADD_HL_SP() {reg.HL = ADD_16(reg.HL, reg.SP);}

void ADD_SP_IM8() {

    reg.Z_FLAG = reg.N_FLAG = 0;
    int8_t s8 = SIGNED_IM_8_BIT;    
    reg.SP += s8;

    //TODO find out why this works :|
    uint16_t temp = (reg.SP - s8) ^ s8 ^ reg.SP;
    reg.C_FLAG = !!(temp & 0x100);
    reg.H_FLAG = !!(temp & 0x10);
}


/* 16 bit register Increments */

void INC_BC(){reg.BC++;}
void INC_DE(){reg.DE++;}
void INC_HL(){reg.HL++;}
void INC_SP(){reg.SP++;}

/* 16 bit register Decrements */

void DEC_BC(){reg.BC--;}
void DEC_DE(){reg.DE--;}
void DEC_HL(){reg.HL--;}
void DEC_SP(){reg.SP--;}



/*  Miscellaneous instructions */

/*  Swap upper and lower nibbles of n */
static inline uint8_t SWAP_n(uint8_t val)
{
    val = ((val & 0xF) << 4) | (val >> 4);
    reg.Z_FLAG = !val; //Check if result is 0
    reg.C_FLAG = reg.H_FLAG = reg.N_FLAG = 0;
    return val;
}

void SWAP_A(){reg.A = SWAP_n(reg.A);}
void SWAP_B(){reg.B = SWAP_n(reg.B);}
void SWAP_C(){reg.C = SWAP_n(reg.C);}
void SWAP_D(){reg.D = SWAP_n(reg.D);}
void SWAP_E(){reg.E = SWAP_n(reg.E);}
void SWAP_H(){reg.H = SWAP_n(reg.H);}
void SWAP_L(){reg.L = SWAP_n(reg.L);}

void SWAP_memHL() {
    update_timers(4);
    uint8_t result = SWAP_n(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}

/*  Decimal adjust register A so that correct 
 *  representation of  binary encoded decimal is obtained */
void DAA() {   
    
    if (!reg.N_FLAG) {
        if (reg.C_FLAG || reg.A > 0x99) {
            reg.A += 0x60;
            reg.C_FLAG = 1;
        }
        if (reg.H_FLAG || (reg.A & 0xF) > 0x9) {
            reg.A += 0x06;
            reg.H_FLAG = 0;
        }
    } else if (reg.H_FLAG && reg.C_FLAG) {
        reg.A += 0x9A;
        reg.H_FLAG = 0;
    } else if (reg.C_FLAG) {
        reg.A += 0xA0;
    } else if (reg.H_FLAG) {
        reg.A += 0xFA;
        reg.H_FLAG = 0;
    }
    reg.Z_FLAG = !reg.A;
}   



/* Flips all bits in register A */
void CPL() {reg.A = ~reg.A; reg.N_FLAG = 1; reg.H_FLAG = 1;}


/*  Flips carry flag  */
void CCF() {reg.C_FLAG = !reg.C_FLAG; reg.H_FLAG = 0; reg.N_FLAG = 0;}

/*  Sets carry flag */
void SCF() {reg.C_FLAG = 1; reg.H_FLAG = 0; reg.N_FLAG = 0;}


/*No operation */
void NOP() {}


/*  Halt CPU until interrupt */
void HALT() {halt = 1;}

/*  Halt CPU and LCD until button pressed */
void STOP() {stop = 1;}


/*  Disable interrupts */
void DI() {interrupts_enabled = 0;}

/*  Enable interrupts 
 *  interrupts enabled after the next instruction
 *  so set a timer to let the cpu know this*/
void EI() {interrupts_enabled_timer = 1;}


/*  Rotates and shifts 
 *  NOTE: DP Gameboy Manual states
 *  for RLCA, RLA, RRCA and RRA the zero
 *  flag is set if A becomes 0. All other
 *  sources state the zero flag is reset
 *  regardless */

/* Rotate A left, Old msb to carry flag and bit 0 */
void RLCA()
{
    reg.C_FLAG = reg.A >> 7; /*  Carry flag stores msb */
    reg.A = (reg.A << 1) | reg.C_FLAG;
    reg.Z_FLAG  = reg.N_FLAG = reg.H_FLAG = 0;
    
}

/*  Rotate A left, Old C_Flag goes to bit 0, bit 7 goes to C_Flag */
void RLA()
{
   unsigned int temp = reg.A >> 7;
   reg.A = (reg.A << 1) | reg.C_FLAG;
   reg.C_FLAG = temp;
   reg.Z_FLAG = reg.N_FLAG = reg.H_FLAG = 0;

}


/*  Rotate A right, old bit 0 goes to carry flag and bit 7*/
void RRCA()
{
    reg.C_FLAG = (reg.A & 0x01);
    reg.A = (reg.A >> 1) | (reg.C_FLAG << 7);
    reg.Z_FLAG = reg.N_FLAG = reg.H_FLAG = 0; 
}


void RRA()
{
    unsigned int temp = (reg.A & 0x01);
    reg.A = (reg.A >> 1) | (reg.C_FLAG << 7);
    reg.C_FLAG = temp;
    reg.Z_FLAG = reg.H_FLAG = reg.N_FLAG = 0;
}


/* Extended instructions */
/*Rotate n left. Old bit 7 to Carry flag*/
static inline uint8_t RLC_N(uint8_t val)
{
   reg.C_FLAG = val >> 7;
   val = val << 1 | reg.C_FLAG;
   reg.Z_FLAG = !val;
   reg.N_FLAG = reg.H_FLAG = 0;
   return val;
}

/*  8 cycles */
void RLC_A() { reg.A = RLC_N(reg.A);}
void RLC_B() { reg.B = RLC_N(reg.B);}
void RLC_C() { reg.C = RLC_N(reg.C);}
void RLC_D() { reg.D = RLC_N(reg.D);}
void RLC_E() { reg.E = RLC_N(reg.E);}
void RLC_H() { reg.H = RLC_N(reg.H);}
void RLC_L() { reg.L = RLC_N(reg.L);}
/* 16 cycles */
void RLC_memHL() {
    update_timers(4);
    uint8_t res = RLC_N(get_mem(reg.HL));
    update_timers(4); 
    set_mem(reg.HL, res);
}




/*  Rotate n left through carry flag */
static inline uint8_t RL_N(uint8_t val) 
{
   uint8_t temp = reg.C_FLAG; 
   reg.C_FLAG = val >> 7;
   val = val << 1 | temp;
   reg.Z_FLAG = !val;
   reg.N_FLAG = reg.H_FLAG = 0;
   return val;

}

void RL_A() {reg.A = RL_N(reg.A);}
void RL_B() {reg.B = RL_N(reg.B);}
void RL_C() {reg.C = RL_N(reg.C);}
void RL_D() {reg.D = RL_N(reg.D);}
void RL_E() {reg.E = RL_N(reg.E);}
void RL_H() {reg.H = RL_N(reg.H);}
void RL_L() {reg.L = RL_N(reg.L);}

void RL_memHL() {
    update_timers(4);
    uint8_t result = RL_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}


/* Rotate N right, Old bit 0 to Carry flag */
static inline uint8_t RRC_N(uint8_t val)
{
    reg.C_FLAG = val & 0x1;
    val = (val >> 1) | (reg.C_FLAG << 7);
    reg.Z_FLAG = !val;
    reg.N_FLAG = reg.H_FLAG = 0;
    return val;
}

/*  4 cyles */
void RRC_A() {reg.A = RRC_N(reg.A);}
void RRC_B() {reg.B = RRC_N(reg.B);}
void RRC_C() {reg.C = RRC_N(reg.C);}
void RRC_D() {reg.D = RRC_N(reg.D);}
void RRC_E() {reg.E = RRC_N(reg.E);}
void RRC_H() {reg.H = RRC_N(reg.H);}
void RRC_L() {reg.L = RRC_N(reg.L);}
/*  12 cycles */
void RRC_memHL() {
    update_timers(4);
    uint8_t result = RRC_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}


/*  Rotate N right through Carry flag */
static inline uint8_t RR_N(uint8_t val)
{
    uint8_t temp = val & 0x1;
    val = (val >> 1) | (reg.C_FLAG << 7);
    reg.C_FLAG = temp;
    reg.Z_FLAG = !val;
    reg.N_FLAG = reg.H_FLAG = 0;
    return val;
}

/*  4 cyles */
void RR_A() {reg.A = RR_N(reg.A);}
void RR_B() {reg.B = RR_N(reg.B);}
void RR_C() {reg.C = RR_N(reg.C);}
void RR_D() {reg.D = RR_N(reg.D);}
void RR_E() {reg.E = RR_N(reg.E);}
void RR_H() {reg.H = RR_N(reg.H);}
void RR_L() {reg.L = RR_N(reg.L);}
/*  12 cycles */
void RR_memHL() {
    update_timers(4);
    uint8_t result = RR_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}



static inline uint8_t SLA_N(uint8_t val)
{
    reg.C_FLAG = val >> 7;
    val <<= 1;
    reg.Z_FLAG = !val;
    reg.N_FLAG = reg.H_FLAG = 0;
    return val;

}


/*  4 cyles */
void SLA_A() {reg.A = SLA_N(reg.A);}
void SLA_B() {reg.B = SLA_N(reg.B);}
void SLA_C() {reg.C = SLA_N(reg.C);}
void SLA_D() {reg.D = SLA_N(reg.D);}
void SLA_E() {reg.E = SLA_N(reg.E);}
void SLA_H() {reg.H = SLA_N(reg.H);}
void SLA_L() {reg.L = SLA_N(reg.L);}
/*  12 cycles */
void SLA_memHL() {
    update_timers(4);
    uint8_t result = SLA_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}



/* Shift n right into Carry. MSB unchanged.*/
static inline uint8_t SRA_N(uint8_t val)
{
    reg.C_FLAG = val & 0x1;
    val = (val >> 1) | (val & 0x80);
    reg.Z_FLAG = !val;
    reg.N_FLAG = reg.H_FLAG = 0;
    return val;
}

/*  4 cyles */
void SRA_A() {reg.A = SRA_N(reg.A);}
void SRA_B() {reg.B = SRA_N(reg.B);}
void SRA_C() {reg.C = SRA_N(reg.C);}
void SRA_D() {reg.D = SRA_N(reg.D);}
void SRA_E() {reg.E = SRA_N(reg.E);}
void SRA_H() {reg.H = SRA_N(reg.H);}
void SRA_L() {reg.L = SRA_N(reg.L);}
/*  12 cycles */
void SRA_memHL() {
    update_timers(4);
    uint8_t result = SRA_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}



/* Shift n right into Carry, MSB set to 0 */
static inline uint8_t SRL_N(uint8_t val)
{
    reg.C_FLAG = val & 0x1;
    val >>= 1;
    reg.Z_FLAG = !val;
    reg.N_FLAG = reg.H_FLAG = 0;
    return val;
}

/*  8 cyles */
void SRL_A() {reg.A = SRL_N(reg.A);}
void SRL_B() {reg.B = SRL_N(reg.B);}
void SRL_C() {reg.C = SRL_N(reg.C);}
void SRL_D() {reg.D = SRL_N(reg.D);}
void SRL_E() {reg.E = SRL_N(reg.E);}
void SRL_H() {reg.H = SRL_N(reg.H);}
void SRL_L() {reg.L = SRL_N(reg.L);}
/*  16 cycles */
void SRL_memHL() {
    update_timers(4);
    uint8_t result = SRL_N(get_mem(reg.HL));
    update_timers(4);
    set_mem(reg.HL, result);
}

/**** Bit Opcodes ****/

/* TODO Test bit b in register r */
static inline void BIT_b_r(uint8_t val, uint8_t bit)
{
    reg.H_FLAG = 1;
    reg.N_FLAG = 0;
    reg.Z_FLAG = !((val >> bit) & 0x1); 
}

/*  8 cyles */
void BIT_A_0() {BIT_b_r(reg.A, 0);}
void BIT_A_1() {BIT_b_r(reg.A, 1);}
void BIT_A_2() {BIT_b_r(reg.A, 2);}
void BIT_A_3() {BIT_b_r(reg.A, 3);}
void BIT_A_4() {BIT_b_r(reg.A, 4);}
void BIT_A_5() {BIT_b_r(reg.A, 5);}
void BIT_A_6() {BIT_b_r(reg.A, 6);}
void BIT_A_7() {BIT_b_r(reg.A, 7);}

void BIT_B_0() {BIT_b_r(reg.B, 0);}
void BIT_B_1() {BIT_b_r(reg.B, 1);}
void BIT_B_2() {BIT_b_r(reg.B, 2);}
void BIT_B_3() {BIT_b_r(reg.B, 3);}
void BIT_B_4() {BIT_b_r(reg.B, 4);}
void BIT_B_5() {BIT_b_r(reg.B, 5);}
void BIT_B_6() {BIT_b_r(reg.B, 6);}
void BIT_B_7() {BIT_b_r(reg.B, 7);}

void BIT_C_0() {BIT_b_r(reg.C, 0);}
void BIT_C_1() {BIT_b_r(reg.C, 1);}
void BIT_C_2() {BIT_b_r(reg.C, 2);}
void BIT_C_3() {BIT_b_r(reg.C, 3);}
void BIT_C_4() {BIT_b_r(reg.C, 4);}
void BIT_C_5() {BIT_b_r(reg.C, 5);}
void BIT_C_6() {BIT_b_r(reg.C, 6);}
void BIT_C_7() {BIT_b_r(reg.C, 7);}

void BIT_D_0() {BIT_b_r(reg.D, 0);}
void BIT_D_1() {BIT_b_r(reg.D, 1);}
void BIT_D_2() {BIT_b_r(reg.D, 2);}
void BIT_D_3() {BIT_b_r(reg.D, 3);}
void BIT_D_4() {BIT_b_r(reg.D, 4);}
void BIT_D_5() {BIT_b_r(reg.D, 5);}
void BIT_D_6() {BIT_b_r(reg.D, 6);}
void BIT_D_7() {BIT_b_r(reg.D, 7);}

void BIT_E_0() {BIT_b_r(reg.E, 0);}
void BIT_E_1() {BIT_b_r(reg.E, 1);}
void BIT_E_2() {BIT_b_r(reg.E, 2);}
void BIT_E_3() {BIT_b_r(reg.E, 3);}
void BIT_E_4() {BIT_b_r(reg.E, 4);}
void BIT_E_5() {BIT_b_r(reg.E, 5);}
void BIT_E_6() {BIT_b_r(reg.E, 6);}
void BIT_E_7() {BIT_b_r(reg.E, 7);}

void BIT_H_0() {BIT_b_r(reg.H, 0);}
void BIT_H_1() {BIT_b_r(reg.H, 1);}
void BIT_H_2() {BIT_b_r(reg.H, 2);}
void BIT_H_3() {BIT_b_r(reg.H, 3);}
void BIT_H_4() {BIT_b_r(reg.H, 4);}
void BIT_H_5() {BIT_b_r(reg.H, 5);}
void BIT_H_6() {BIT_b_r(reg.H, 6);}
void BIT_H_7() {BIT_b_r(reg.H, 7);}

void BIT_L_0() {BIT_b_r(reg.L, 0);}
void BIT_L_1() {BIT_b_r(reg.L, 1);}
void BIT_L_2() {BIT_b_r(reg.L, 2);}
void BIT_L_3() {BIT_b_r(reg.L, 3);}
void BIT_L_4() {BIT_b_r(reg.L, 4);}
void BIT_L_5() {BIT_b_r(reg.L, 5);}
void BIT_L_6() {BIT_b_r(reg.L, 6);}
void BIT_L_7() {BIT_b_r(reg.L, 7);}

/*  16 cycles */
void BIT_memHL_0() { update_timers(4); BIT_b_r(get_mem(reg.HL),0);}
void BIT_memHL_1() { update_timers(4); BIT_b_r(get_mem(reg.HL),1);}
void BIT_memHL_2() { update_timers(4); BIT_b_r(get_mem(reg.HL),2);}
void BIT_memHL_3() { update_timers(4); BIT_b_r(get_mem(reg.HL),3);}
void BIT_memHL_4() { update_timers(4); BIT_b_r(get_mem(reg.HL),4);}
void BIT_memHL_5() { update_timers(4); BIT_b_r(get_mem(reg.HL),5);}
void BIT_memHL_6() { update_timers(4); BIT_b_r(get_mem(reg.HL),6);}
void BIT_memHL_7() { update_timers(4); BIT_b_r(get_mem(reg.HL),7);}



/*  Set bit b in register r */
static inline uint8_t SET_b_r(uint8_t const val, uint8_t const bit)
{
    return val | (0x1 << bit);
}

static inline void SET_b_mem(uint16_t const addr, uint8_t const bit) {

    update_timers(4);
    uint8_t result = SET_b_r(get_mem(addr), bit);
    update_timers(4);
    set_mem(addr, result);
}

/*  8 cyles */
void SET_A_0() {reg.A  = SET_b_r(reg.A, 0);}
void SET_A_1() {reg.A  = SET_b_r(reg.A, 1);}
void SET_A_2() {reg.A  = SET_b_r(reg.A, 2);}
void SET_A_3() {reg.A  = SET_b_r(reg.A, 3);}
void SET_A_4() {reg.A  = SET_b_r(reg.A, 4);}
void SET_A_5() {reg.A  = SET_b_r(reg.A, 5);}
void SET_A_6() {reg.A  = SET_b_r(reg.A, 6);}
void SET_A_7() {reg.A  = SET_b_r(reg.A, 7);}

void SET_B_0() {reg.B  = SET_b_r(reg.B, 0);}
void SET_B_1() {reg.B  = SET_b_r(reg.B, 1);}
void SET_B_2() {reg.B  = SET_b_r(reg.B, 2);}
void SET_B_3() {reg.B  = SET_b_r(reg.B, 3);}
void SET_B_4() {reg.B  = SET_b_r(reg.B, 4);}
void SET_B_5() {reg.B  = SET_b_r(reg.B, 5);}
void SET_B_6() {reg.B  = SET_b_r(reg.B, 6);}
void SET_B_7() {reg.B  = SET_b_r(reg.B, 7);}

void SET_C_0() {reg.C = SET_b_r(reg.C, 0);}
void SET_C_1() {reg.C = SET_b_r(reg.C, 1);}
void SET_C_2() {reg.C  = SET_b_r(reg.C, 2);}
void SET_C_3() {reg.C  = SET_b_r(reg.C, 3);}
void SET_C_4() {reg.C  = SET_b_r(reg.C, 4);}
void SET_C_5() {reg.C  = SET_b_r(reg.C, 5);}
void SET_C_6() {reg.C  = SET_b_r(reg.C, 6);}
void SET_C_7() {reg.C  = SET_b_r(reg.C, 7);}

void SET_D_0() {reg.D  = SET_b_r(reg.D, 0);}
void SET_D_1() {reg.D  = SET_b_r(reg.D, 1);}
void SET_D_2() {reg.D  = SET_b_r(reg.D, 2);}
void SET_D_3() {reg.D  = SET_b_r(reg.D, 3);}
void SET_D_4() {reg.D  = SET_b_r(reg.D, 4);}
void SET_D_5() {reg.D  = SET_b_r(reg.D, 5);}
void SET_D_6() {reg.D  = SET_b_r(reg.D, 6);}
void SET_D_7() {reg.D  = SET_b_r(reg.D, 7);}

void SET_E_0() {reg.E  = SET_b_r(reg.E, 0);}
void SET_E_1() {reg.E  = SET_b_r(reg.E, 1);}
void SET_E_2() {reg.E  = SET_b_r(reg.E, 2);}
void SET_E_3() {reg.E  = SET_b_r(reg.E, 3);}
void SET_E_4() {reg.E  = SET_b_r(reg.E, 4);}
void SET_E_5() {reg.E  = SET_b_r(reg.E, 5);}
void SET_E_6() {reg.E  = SET_b_r(reg.E, 6);}
void SET_E_7() {reg.E  = SET_b_r(reg.E, 7);}

void SET_H_0() {reg.H  = SET_b_r(reg.H, 0);}
void SET_H_1() {reg.H  = SET_b_r(reg.H, 1);}
void SET_H_2() {reg.H  = SET_b_r(reg.H, 2);}
void SET_H_3() {reg.H  = SET_b_r(reg.H, 3);}
void SET_H_4() {reg.H  = SET_b_r(reg.H, 4);}
void SET_H_5() {reg.H  = SET_b_r(reg.H, 5);}
void SET_H_6() {reg.H  = SET_b_r(reg.H, 6);}
void SET_H_7() {reg.H  = SET_b_r(reg.H, 7);}

void SET_L_0() {reg.L = SET_b_r(reg.L, 0);}
void SET_L_1() {reg.L = SET_b_r(reg.L, 1);}
void SET_L_2() {reg.L = SET_b_r(reg.L, 2);}
void SET_L_3() {reg.L = SET_b_r(reg.L, 3);}
void SET_L_4() {reg.L = SET_b_r(reg.L, 4);}
void SET_L_5() {reg.L = SET_b_r(reg.L, 5);}
void SET_L_6() {reg.L = SET_b_r(reg.L, 6);}
void SET_L_7() {reg.L = SET_b_r(reg.L, 7);}

/*  16 cycles */
void SET_memHL_0() {SET_b_mem(reg.HL,0);}
void SET_memHL_1() {SET_b_mem(reg.HL,1);}
void SET_memHL_2() {SET_b_mem(reg.HL,2);}
void SET_memHL_3() {SET_b_mem(reg.HL,3);}
void SET_memHL_4() {SET_b_mem(reg.HL,4);}
void SET_memHL_5() {SET_b_mem(reg.HL,5);}
void SET_memHL_6() {SET_b_mem(reg.HL,6);}
void SET_memHL_7() {SET_b_mem(reg.HL,7);}



/*  Reset bit b in register r */
static inline uint8_t RES_b_r(uint8_t val, uint8_t bit)
{
    return val & ~(0x1 << bit);
}

static inline void RES_b_mem(uint16_t addr, uint8_t bit) {

    update_timers(4);
    uint8_t result = RES_b_r(get_mem(addr), bit);
    update_timers(4);
    set_mem(addr, result);
}



void RES_A_0() {reg.A = RES_b_r(reg.A, 0);}
void RES_A_1() {reg.A = RES_b_r(reg.A, 1);}
void RES_A_2() {reg.A = RES_b_r(reg.A, 2);}
void RES_A_3() {reg.A = RES_b_r(reg.A, 3);}
void RES_A_4() {reg.A = RES_b_r(reg.A, 4);}
void RES_A_5() {reg.A = RES_b_r(reg.A, 5);}
void RES_A_6() {reg.A = RES_b_r(reg.A, 6);}
void RES_A_7() {reg.A = RES_b_r(reg.A, 7);}

void RES_B_0() {reg.B = RES_b_r(reg.B, 0);}
void RES_B_1() {reg.B = RES_b_r(reg.B, 1);}
void RES_B_2() {reg.B = RES_b_r(reg.B, 2);}
void RES_B_3() {reg.B = RES_b_r(reg.B, 3);}
void RES_B_4() {reg.B = RES_b_r(reg.B, 4);}
void RES_B_5() {reg.B = RES_b_r(reg.B, 5);}
void RES_B_6() {reg.B = RES_b_r(reg.B, 6);}
void RES_B_7() {reg.B = RES_b_r(reg.B, 7);}

void RES_C_0() {reg.C = RES_b_r(reg.C, 0);}
void RES_C_1() {reg.C = RES_b_r(reg.C, 1);}
void RES_C_2() {reg.C = RES_b_r(reg.C, 2);}
void RES_C_3() {reg.C = RES_b_r(reg.C, 3);}
void RES_C_4() {reg.C = RES_b_r(reg.C, 4);}
void RES_C_5() {reg.C = RES_b_r(reg.C, 5);}
void RES_C_6() {reg.C = RES_b_r(reg.C, 6);}
void RES_C_7() {reg.C = RES_b_r(reg.C, 7);}

void RES_D_0() {reg.D = RES_b_r(reg.D, 0);}
void RES_D_1() {reg.D = RES_b_r(reg.D, 1);}
void RES_D_2() {reg.D = RES_b_r(reg.D, 2);}
void RES_D_3() {reg.D = RES_b_r(reg.D, 3);}
void RES_D_4() {reg.D = RES_b_r(reg.D, 4);}
void RES_D_5() {reg.D = RES_b_r(reg.D, 5);}
void RES_D_6() {reg.D = RES_b_r(reg.D, 6);}
void RES_D_7() {reg.D = RES_b_r(reg.D, 7);}

void RES_E_0() {reg.E = RES_b_r(reg.E, 0);}
void RES_E_1() {reg.E = RES_b_r(reg.E, 1);}
void RES_E_2() {reg.E = RES_b_r(reg.E, 2);}
void RES_E_3() {reg.E = RES_b_r(reg.E, 3);}
void RES_E_4() {reg.E = RES_b_r(reg.E, 4);}
void RES_E_5() {reg.E = RES_b_r(reg.E, 5);}
void RES_E_6() {reg.E = RES_b_r(reg.E, 6);}
void RES_E_7() {reg.E = RES_b_r(reg.E, 7);}

void RES_H_0() {reg.H = RES_b_r(reg.H, 0);}
void RES_H_1() {reg.H = RES_b_r(reg.H, 1);}
void RES_H_2() {reg.H = RES_b_r(reg.H, 2);}
void RES_H_3() {reg.H = RES_b_r(reg.H, 3);}
void RES_H_4() {reg.H = RES_b_r(reg.H, 4);}
void RES_H_5() {reg.H = RES_b_r(reg.H, 5);}
void RES_H_6() {reg.H = RES_b_r(reg.H, 6);}
void RES_H_7() {reg.H = RES_b_r(reg.H, 7);}

void RES_L_0() {reg.L = RES_b_r(reg.L, 0);}
void RES_L_1() {reg.L = RES_b_r(reg.L, 1);}
void RES_L_2() {reg.L = RES_b_r(reg.L, 2);}
void RES_L_3() {reg.L = RES_b_r(reg.L, 3);}
void RES_L_4() {reg.L = RES_b_r(reg.L, 4);}
void RES_L_5() {reg.L = RES_b_r(reg.L, 5);}
void RES_L_6() {reg.L = RES_b_r(reg.L, 6);}
void RES_L_7() {reg.L = RES_b_r(reg.L, 7);}

/*  16 cycles */
void RES_memHL_0() {RES_b_mem(reg.HL,0);}
void RES_memHL_1() {RES_b_mem(reg.HL,1);}
void RES_memHL_2() {RES_b_mem(reg.HL,2);}
void RES_memHL_3() {RES_b_mem(reg.HL,3);}
void RES_memHL_4() {RES_b_mem(reg.HL,4);}
void RES_memHL_5() {RES_b_mem(reg.HL,5);}
void RES_memHL_6() {RES_b_mem(reg.HL,6);}
void RES_memHL_7() {RES_b_mem(reg.HL,7);}




/**** Jumps ****/

/* Jump to immediate 2 byte address */
void JP_nn() { reg.PC = IMMEDIATE_16_BIT; }

/*  Jump to address n if flag condition holds */

void JP_NZ_nn() { ins[0xC2].cycles = !reg.Z_FLAG ? (JP_nn(), 16) : 12; }
void JP_Z_nn()  { ins[0xCA].cycles =  reg.Z_FLAG ? (JP_nn(), 16) : 12; }
void JP_NC_nn() { ins[0xD2].cycles = !reg.C_FLAG ? (JP_nn(), 16) : 12; }
void JP_C_nn()  { ins[0xDA].cycles =  reg.C_FLAG ? (JP_nn(), 16) : 12; }


/*  Jump to address contained in HL */
void JP_HL() { reg.PC = reg.HL; }



/*  Add 8 bit immediate value as signed int to current address
 *  and jump to it */
void JR_n() { reg.PC += SIGNED_IM_8_BIT;}

/*  If following flag conditions are true
 *  add 8 bit immediate to pc */

void JR_NZ_n() { ins[0x20].cycles = !reg.Z_FLAG ? (JR_n(), 12) : 8; }
void JR_Z_n()  { ins[0x28].cycles =  reg.Z_FLAG ? (JR_n(), 12) : 8; }
void JR_NC_n() { ins[0x30].cycles = !reg.C_FLAG ? (JR_n(), 12) : 8; }
void JR_C_n()  { ins[0x38].cycles =  reg.C_FLAG ? (JR_n(), 12) : 8; }




/**** Calls ****/

/*  Push address of next instruction onto stack
 *  then jump to address nn */
void CALL_nn()
{
    PUSH(reg.PC);
    reg.PC = IMMEDIATE_16_BIT;
}

/*  Call if flag is set/unset */
void CALL_NZ_nn() { ins[0xC4].cycles = !reg.Z_FLAG ? (CALL_nn(), 24) : 12; }
void CALL_Z_nn()  { ins[0xCC].cycles =  reg.Z_FLAG ? (CALL_nn(), 24) : 12; }
void CALL_NC_nn() { ins[0xD4].cycles = !reg.C_FLAG ? (CALL_nn(), 24) : 12; }
void CALL_C_nn()  { ins[0xDC].cycles =  reg.C_FLAG ? (CALL_nn(), 24) : 12; }





/**** Restarts ****/
/*  Push present address onto stack
 *  Jump to addres $0000 + n */
void restart(uint8_t addr)
{
    PUSH(reg.PC);
    reg.PC = addr;
}

void RST_00() {restart(0x00);}
void RST_08() {restart(0x08);}
void RST_10() {restart(0x10);}
void RST_18() {restart(0x18);}
void RST_20() {restart(0x20);}
void RST_28() {restart(0x28);}
void RST_30() {restart(0x30);}
void RST_38() {restart(0x38);}


/**** Returns ****/

/*  Pop two bytes from stack and jump to that addr */
void RET() { POP(&reg.PC);}

// Return if flags are set
void RET_NZ() { ins[0xC0].cycles = !reg.Z_FLAG ? (RET(), 20) : 8; } 
void RET_Z()  { ins[0xC8].cycles =  reg.Z_FLAG ? (RET(), 20) : 8; }
void RET_NC() { ins[0xD0].cycles = !reg.C_FLAG ? (RET(), 20) : 8; }
void RET_C()  { ins[0xD8].cycles =  reg.C_FLAG ? (RET(), 20) : 8; }



// Return and enable master interrupts
void RETI() {
    RET();
    EI();
}

/* ***************************************** */


Instruction ins[UINT8_MAX + 1] = {
    
    // 0x00 - 0x0F
    {4, NOP}, {12, LD_BC_IM}, {8, LD_memBC_A}, {8, INC_BC},     
    {4, INC_B}, {4, DEC_B}, {8, LD_B_IM}, {4, RLCA},     
    {20, LD_nn_SP}, {8, ADD_HL_BC}, {8, LD_A_memBC}, {8, DEC_BC},     
    {4, INC_C}, {4, DEC_C}, {8, LD_C_IM}, {4, RRCA},
    
    //0x10 - 0x1F
    {0, STOP}, {12, LD_DE_IM}, {8, LD_memDE_A}, {8, INC_DE},     
    {4, INC_D}, {4, DEC_D}, {8, LD_D_IM}, {4, RLA},
    {12, JR_n}, {8, ADD_HL_DE}, {8, LD_A_memDE}, {8, DEC_DE},
    {4, INC_E}, {4, DEC_E}, {8, LD_E_IM}, {4, RRA},

    //0x20 - 0x2F
    {8, JR_NZ_n}, {12, LD_HL_IM}, {8, LDI_HL_A}, {8, INC_HL},
    {4, INC_H}, {4, DEC_H}, {8, LD_H_IM}, {4, DAA},
    {8, JR_Z_n}, {8, ADD_HL_HL}, {8, LDI_A_HL}, {8, DEC_HL},
    {4, INC_L}, {4, DEC_L}, {8, LD_L_IM}, {4,CPL},

    //0x30 - 0x3F
    {8, JR_NC_n}, {12, LD_SP_IM}, {8, LDD_HL_A}, {8, INC_SP},
    {12, INC_memHL}, {12, DEC_memHL}, {12, LD_memHL_n}, {4, SCF},
    {8, JR_C_n}, {8, ADD_HL_SP}, {8, LDD_A_HL}, {8, DEC_SP},
    {4, INC_A}, {4, DEC_A}, {8, LD_A_IM}, {4, CCF},

    //0x40 - 0x4F
    {4, LD_B_B}, {4, LD_B_C}, {4, LD_B_D}, {4, LD_B_E},     
    {4 ,LD_B_H}, {4, LD_B_L}, {8, LD_B_memHL}, {4 ,LD_B_A},
    {4 ,LD_C_B}, {4 ,LD_C_C}, {4, LD_C_D}, {4, LD_C_E},
    {4 ,LD_C_H}, {4 ,LD_C_L}, {8 ,LD_C_memHL}, {4 ,LD_C_A},
                          
    //0x50 - 0x5F 
    {4 ,LD_D_B}, {4, LD_D_C}, {4, LD_D_D}, {4, LD_D_E},
    {4, LD_D_H}, {4 ,LD_D_L}, {8 ,LD_D_memHL}, {4, LD_D_A},
    {4, LD_E_B}, {4 ,LD_E_C}, {4, LD_E_D}, {4, LD_E_E},
    {4, LD_E_H}, {4, LD_E_L}, {8 ,LD_E_memHL}, {4 ,LD_E_A},
                              
    //0x60 - 0x6F 
    {4 ,LD_H_B}, {4, LD_H_C}, {4, LD_H_D}, {4 ,LD_H_E}, 
    {4, LD_H_H}, {4 ,LD_H_L}, {8 ,LD_H_memHL}, {4, LD_H_A},
    {4, LD_L_B}, {4, LD_L_C}, {4, LD_L_D}, {4, LD_L_E},
    {4 ,LD_L_H}, {4 ,LD_L_L}, {8 ,LD_L_memHL}, {4 ,LD_L_A},

    //0x70 - 0x7F
    {8, LD_memHL_B}, {8, LD_memHL_C}, {8, LD_memHL_D}, {8, LD_memHL_E}, 
    {8, LD_memHL_H}, {8 ,LD_memHL_L}, {0, HALT}, {8, LD_memHL_A}, 
    {4, LD_A_B}, {4, LD_A_C}, {4, LD_A_D}, {4, LD_A_E},
    {4, LD_A_H}, {4, LD_A_L}, {8, LD_A_memHL}, {4, LD_A_A},

    //0x80 - 0x8F
    {4, ADD_A_B}, {4, ADD_A_C}, {4, ADD_A_D},  {4, ADD_A_E},    
    {4, ADD_A_H}, {4, ADD_A_L}, {8, ADD_A_memHL}, {4, ADD_A_A},    
    {4, ADC_A_B}, {4, ADC_A_C}, {4, ADC_A_D}, {4, ADC_A_E},    
    {4, ADC_A_H}, {4, ADC_A_L}, {8, ADC_A_memHL}, {4 ,ADC_A_A},

    //0x90 - 0x9F
    {4, SUB_A_B}, {4, SUB_A_C}, {4, SUB_A_D}, {4, SUB_A_E},
    {4, SUB_A_H}, {4, SUB_A_L}, {8, SUB_A_memHL}, {4, SUB_A_A},
    {4, SBC_A_B}, {4, SBC_A_C}, {4, SBC_A_D}, {4, SBC_A_E}, 
    {4, SBC_A_H}, {4, SBC_A_L}, {8 ,SBC_A_memHL}, {4, SBC_A_A},
    
    //0xA0 - 0xAF
    {4, AND_A_B}, {4, AND_A_C}, {4, AND_A_D}, {4, AND_A_E},    
    {4, AND_A_H}, {4, AND_A_L}, {8, AND_A_memHL}, {4, AND_A_A},
    {4, XOR_A_B}, {4, XOR_A_C}, {4, XOR_A_D}, {4, XOR_A_E},
    {4, XOR_A_H}, {4, XOR_A_L}, {8, XOR_A_memHL}, {4, XOR_A_A},

    //0xB0 - 0xBF
    {4, OR_A_B}, {4, OR_A_C}, {4, OR_A_D}, {4, OR_A_E},
    {4, OR_A_H}, {4, OR_A_L}, {8, OR_A_memHL}, {4, OR_A_A},
    {4, CP_A_B}, {4, CP_A_C}, {4, CP_A_D}, {4, CP_A_E},  
    {4, CP_A_H}, {4, CP_A_L}, {8, CP_A_memHL,}, {4, CP_A_A},

    //0xC0 - 0xCF
    {8, RET_NZ}, {12, POP_BC}, {12, JP_NZ_nn}, {16, JP_nn},
    {12, CALL_NZ_nn}, {16, PUSH_BC}, {8, ADD_A_Im8}, {16, RST_00},
    {8, RET_Z}, {16, RET}, {12, JP_Z_nn}, {0, invalid_op},
    {12, CALL_Z_nn}, {24, CALL_nn}, {8, ADC_A_Im8}, {16, RST_08},

    //0xD0 - 0xDF
    {8, RET_NC}, {12, POP_DE}, {12, JP_NC_nn}, {0, invalid_op},
    {12, CALL_NC_nn}, {16, PUSH_DE}, {8, SUB_A_Im8}, {16, RST_10},
    {8, RET_C}, {16, RETI}, {16, JP_C_nn}, {0,  invalid_op}, 
    {12, CALL_C_nn}, {0,  invalid_op}, {8,  SBC_A_Im8}, {16 ,RST_18},

    //0xE0 - 0xEF
    {12, LDH_n_A}, {12, POP_HL}, {8, LDH_C_A}, {0, invalid_op},       
    {0, invalid_op}, {16, PUSH_HL}, {8, AND_A_Im8},{16, RST_20},
    {16, ADD_SP_IM8}, {4, JP_HL}, {16, LD_memnn_A}, {0, invalid_op},       
    {0, invalid_op}, {0, invalid_op}, {8, XOR_A_Im8}, {16, RST_28},
    
    //0xF0 - 0xFF                                                              
    {12, LDH_A_n}, {12, POP_AF}, {8, LDH_A_C}, {4 ,DI},
    {0,invalid_op}, {16, PUSH_AF}, {8, OR_A_Im8}, {16, RST_30},
    {12, LD_HL_SP_n}, {8, LD_SP_HL}, {16, LD_A_memnn}, {4, EI},
    {0 , invalid_op}, {0, invalid_op}, {8, CP_A_Im8}, {16, RST_38}
};

static Instruction ext_ins[UINT8_MAX+1] = {
    // 0x0X
    {8, RLC_B}, {8, RLC_C}, {8, RLC_D},      {8, RLC_E}, 
    {8, RLC_H}, {8, RLC_L}, {16, RLC_memHL}, {8, RLC_A}, 
    {8, RRC_B}, {8, RRC_C}, {8, RRC_D},      {8, RRC_E},
    {8, RRC_H}, {8, RRC_L}, {16, RRC_memHL}, {8, RRC_A},
    //0x1X
    {8, RL_B}, {8, RL_C}, {8, RL_D},      {8, RL_E},
    {8, RL_H}, {8, RL_L}, {16, RL_memHL}, {8, RL_A}, 
    {8, RR_B}, {8, RR_C}, {8, RR_D},      {8, RR_E},
    {8, RR_H}, {8, RR_L}, {16, RR_memHL}, {8, RR_A},
    //0x2x
    {8, SLA_B}, {8, SLA_C}, {8, SLA_D},      {8, SLA_E},
    {8, SLA_H}, {8, SLA_L}, {16, SLA_memHL}, {8, SLA_A}, 
    {8, SRA_B}, {8, SRA_C}, {8, SRA_D},      {8, SRA_E},
    {8, SRA_H}, {8, SRA_L}, {16, SRA_memHL}, {8, SRA_A},
    //0x3x
    {8, SWAP_B}, {8, SWAP_C}, {8, SWAP_D},      {8, SWAP_E},
    {8, SWAP_H}, {8, SWAP_L}, {16, SWAP_memHL}, {8, SWAP_A}, 
    {8, SRL_B},  {8, SRL_C},  {8, SRL_D},       {8, SRL_E},
    {8, SRL_H},  {8, SRL_L},  {16, SRL_memHL},  {8, SRL_A},
    //0x4x
    {8, BIT_B_0}, {8, BIT_C_0}, {8, BIT_D_0},      {8, BIT_E_0},
    {8, BIT_H_0}, {8, BIT_L_0}, {16, BIT_memHL_0}, {8, BIT_A_0},
    {8, BIT_B_1}, {8, BIT_C_1}, {8, BIT_D_1},      {8, BIT_E_1}, 
    {8, BIT_H_1}, {8, BIT_L_1}, {16, BIT_memHL_1}, {8, BIT_A_1},
    //0x5x
    {8, BIT_B_2}, {8, BIT_C_2}, {8, BIT_D_2},      {8, BIT_E_2}, 
    {8, BIT_H_2}, {8, BIT_L_2}, {16, BIT_memHL_2}, {8, BIT_A_2}, 
    {8, BIT_B_3}, {8, BIT_C_3}, {8, BIT_D_3},      {8, BIT_E_3}, 
    {8, BIT_H_3}, {8, BIT_L_3}, {16, BIT_memHL_3}, {8, BIT_A_3},
    //0x6x
    {8, BIT_B_4}, {8, BIT_C_4}, {8, BIT_D_4},      {8, BIT_E_4}, 
    {8, BIT_H_4}, {8, BIT_L_4}, {16, BIT_memHL_4}, {8, BIT_A_4},
    {8, BIT_B_5}, {8, BIT_C_5}, {8, BIT_D_5},      {8, BIT_E_5}, 
    {8, BIT_H_5}, {8, BIT_L_5}, {16, BIT_memHL_5}, {8, BIT_A_5},
    //0x7x
    {8, BIT_B_6}, {8, BIT_C_6}, {8, BIT_D_6},      {8, BIT_E_6}, 
    {8, BIT_H_6}, {8, BIT_L_6}, {16, BIT_memHL_6}, {8, BIT_A_6}, 
    {8, BIT_B_7}, {8, BIT_C_7}, {8, BIT_D_7},      {8, BIT_E_7},
    {8, BIT_H_7}, {8, BIT_L_7}, {16, BIT_memHL_7}, {8, BIT_A_7},
    //0x8x
    {8, RES_B_0}, {8, RES_C_0}, {8, RES_D_0},      {8, RES_E_0}, 
    {8, RES_H_0}, {8, RES_L_0}, {16, RES_memHL_0}, {8, RES_A_0},
    {8, RES_B_1}, {8, RES_C_1}, {8, RES_D_1},      {8, RES_E_1}, 
    {8, RES_H_1}, {8, RES_L_1}, {16, RES_memHL_1}, {8, RES_A_1},
    //0x9x 
    {8, RES_B_2}, {8, RES_C_2}, {8, RES_D_2},      {8, RES_E_2}, 
    {8, RES_H_2}, {8, RES_L_2}, {16, RES_memHL_2}, {8, RES_A_2}, 
    {8, RES_B_3}, {8, RES_C_3}, {8, RES_D_3},      {8, RES_E_3}, 
    {8, RES_H_3}, {8, RES_L_3}, {16, RES_memHL_3}, {8, RES_A_3},
    //0xAx
    {8, RES_B_4}, {8, RES_C_4}, {8, RES_D_4},      {8, RES_E_4}, 
    {8, RES_H_4}, {8, RES_L_4}, {16, RES_memHL_4}, {8, RES_A_4},
    {8, RES_B_5}, {8, RES_C_5}, {8, RES_D_5},      {8, RES_E_5}, 
    {8, RES_H_5}, {8, RES_L_5}, {16, RES_memHL_5}, {8, RES_A_5},
    //0xBx 
    {8, RES_B_6}, {8, RES_C_6}, {8, RES_D_6},      {8, RES_E_6},
    {8, RES_H_6}, {8, RES_L_6}, {16, RES_memHL_6}, {8, RES_A_6}, 
    {8, RES_B_7}, {8, RES_C_7}, {8, RES_D_7},      {8, RES_E_7}, 
    {8, RES_H_7}, {8, RES_L_7}, {16, RES_memHL_7}, {8, RES_A_7},      
    //0xCx
    {8, SET_B_0}, {8, SET_C_0}, {8, SET_D_0},      {8, SET_E_0}, 
    {8, SET_H_0}, {8, SET_L_0}, {16, SET_memHL_0}, {8, SET_A_0},
    {8, SET_B_1}, {8, SET_C_1}, {8, SET_D_1},      {8, SET_E_1}, 
    {8, SET_H_1}, {8, SET_L_1}, {16, SET_memHL_1}, {8, SET_A_1}, 
    //0xDx
    {8, SET_B_2}, {8, SET_C_2}, {8, SET_D_2},      {8, SET_E_2}, 
    {8, SET_H_2}, {8, SET_L_2}, {16, SET_memHL_2}, {8, SET_A_2}, 
    {8, SET_B_3}, {8, SET_C_3}, {8, SET_D_3},      {8, SET_E_3}, 
    {8, SET_H_3}, {8, SET_L_3}, {16, SET_memHL_3}, {8, SET_A_3},
    //0xEx
    {8, SET_B_4}, {8, SET_C_4}, {8, SET_D_4},      {8, SET_E_4}, 
    {8, SET_H_4}, {8, SET_L_4}, {16, SET_memHL_4}, {8, SET_A_4},
    {8, SET_B_5}, {8, SET_C_5}, {8, SET_D_5},      {8, SET_E_5}, 
    {8, SET_H_5}, {8, SET_L_5}, {16, SET_memHL_5}, {8, SET_A_5},
    //0xFx 
    {8, SET_B_6}, {8, SET_C_6}, {8, SET_D_6},      {8, SET_E_6}, 
    {8, SET_H_6}, {8, SET_L_6}, {16, SET_memHL_6}, {8, SET_A_6}, 
    {8, SET_B_7}, {8, SET_C_7}, {8, SET_D_7},      {8, SET_E_7}, 
    {8, SET_H_7}, {8, SET_L_7}, {16, SET_memHL_7}, {8, SET_A_7}
};


/*  Number of words (bytes) per each instruction
 *  All invallid instruction opcodes are given 1 word 
 *  All extended instructions are 2 bytes, 1 for 0xCB opcode
 *  and another for the specified extended opcode.*/
static int ins_words[UINT8_MAX+1] = {

    1,3,1,1,1,1,2,1,3,1,1,1,1,1,2,1,
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,
    2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,3,3,3,1,2,1,1,1,3,2,3,3,2,1,
    1,1,3,1,3,1,2,1,1,1,3,1,3,1,2,1,
    2,1,1,1,1,1,2,1,2,1,3,1,1,1,2,1,
    2,1,1,1,1,1,2,1,2,1,3,1,1,1,2,1
};




static Instructions instructions = {
    .instruction_set = ins, .words = ins_words, 
    .ext_instruction_set = ext_ins, 
};   
   


int master_interrupts_enabled() { 
    return interrupts_enabled;
}

void master_interrupts_disable() {
    interrupts_enabled = 0;
}

void master_interrupts_enable() {
    interrupts_enabled = 1;
}

void unhalt_cpu() {
    halt = 0;
}

int is_halted() {
    return halt;
}


void reset_cpu() {
    /*  Default starting values for normal GB */
    //TODO A is 0x01 for GB, 0x11 for CGB
    reg.AF = 0x11B0;
    reg.BC = 0x0013;
    reg.DE = 0x00D8;
    reg.HL = 0x014D;
    reg.PC = 0x100;
    reg.SP = 0xFFFE;
}



void print_regs() {
    printf("AF:%x-%x\n",reg.A,reg.F);
    printf("BC:%x-%x\n",reg.B,reg.C);
    printf("DE:%x-%x\n",reg.D,reg.E);
    printf("HL:%x-%x\n",reg.H,reg.L);
    printf("PC:%x\n",reg.PC);
    printf("SP:%x\n",reg.SP);
}


/*  Executes the next processor instruction and returns
 *  the amount of cycles the instruction takes */
int exec_opcode(int skip_bug) {
    
    if (interrupts_enabled_timer) {
            interrupts_enabled = 1;
            interrupts_enabled_timer = 0; //Unset timer
    }
    
    opcode = get_mem(reg.PC); /*  fetch */
 //   dasm_instruction(reg.PC, stdout);
 //   printf("OPCODE:%X, PC:%X SP:%X A:%X F:%X B:%X C:%X D:%X E:%X H:%X L:%X\n",opcode,reg.PC,reg.SP,reg.A,reg.F,reg.B,reg.C,reg.D,reg.E,reg.H,reg.L);    
    if (skip_bug) {
        reg.PC--;
    }

    reg.PC += instructions.words[opcode]; /*  increment PC to next instruction */    
    if (opcode != 0xCB) {
         
        instructions.instruction_set[opcode].operation();
        int cycles = instructions.instruction_set[opcode].cycles;
        update_timers(cycles - timer_cycles_passed);
        timer_cycles_passed = 0;
        return cycles;

    } else { /*  extended instruction */

        opcode = IMMEDIATE_8_BIT;
        instructions.ext_instruction_set[opcode].operation();  
        update_timers(8);     
        return instructions.ext_instruction_set[opcode].cycles;
    }
}
