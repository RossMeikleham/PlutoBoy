/* Modified Z80 GameBoy CPU*/
/* Ross Meikleham */
#include <stdio.h>
#include <stdint.h>

#define WORD_MAX (4096*4096)-1
#define BYTE_MAX 255

uint8_t mem[(4096*4096)-1];


union REGISTERS
{
  struct{uint16_t AF,BC,DE,HL,SP,PC;};
  struct{uint8_t A,F,B,C,D,E,H,L,SPlo,SPhi,PClo,PChi;};
  struct{uint8_t BLANK, Z_FLAG:1, N_FLAG:1, H_FLAG:1, C_FLAG:1 , UNUSED_FLAG:1 ,
   UNUSED_FLAG2:1,UNUSED_FLAG3:1 , UNUSED_FLAG4:1;};

} reg;

#define IMMEDIATE_8_BIT (++reg.PC)
#define IMMEDIATE_16_BIT ((++reg.PC)<< 8) | (++reg.PC)

uint8_t opcode;

/*  Stores address to the function executing the
 *  instruction as well as the number of machine cycles it takes */
/*  typedef struct operation {
    uint8_t m_cycles;
    void *(instruction)();
}

operation operations[] = {}; */


/* 16 bit registers
 *AF //Normally not used as A and F registers needed seperately
 *BC //Used by instructions and code sections, 16 bit counter
 *DE
 *HL //General 16 bit reg 
 *PC //Program Counter
 *SP //Stack Pointer
 */

/*8 bit registers 
 *A // lower 8 bytes of AF 
 *F // higher 8 bytes of AF, Flags register
 *
 *B // lower 8 bytes of BC
 *C // higer 8 bytes of BC
 *
 *D // lower 8 bytes of DE
 *E // higher 8 bytes of ED
 *
 *H // lower 8 bytes of HL
 *L // higher 8 bytes of HL
 *
 */

/*8 bit flags stored in register F
 *Z_FLAG // Zero flag - set if result is zero
 *N_FLAG // Subtraction flag - set if subtraction operation performed
 *H_FLAG // Half Carry flag -
 *C_FLAG // Carry flag -
 */


/************* 8 Bit Load Operations ********************/

/* Load 8 bit immediate value into specified location */
void ins_LD_8_IM(uint8_t *loc) { *loc = mem[IMMEDIATE_8_BIT];}

void ins_LD_A_IM(){ins_LD_n(&reg.A);}

void ins_LD_B_IM(){ins_LD_n(&reg.B);}

void ins_LD_C_IM(){ins_LD_n(&reg.C);}

void ins_LD_D_IM(){ins_LD_n(&reg.D);}

void ins_LD_E_IM(){ins_LD_n(&reg.E);}

void ins_LD_H_IM(){ins_LD_n(&reg.H);}

void ins_LD_L_IM(){ins_LD_n(&reg.L);}



/* Load register value into reg A */
void ins_LD_A_A() {reg.A = reg.A;}

void ins_LD_A_B() {reg.A = reg.B;}

void ins_LD_A_C() {reg.A = reg.C;}

void ins_LD_A_D() {reg.A = reg.D;}

void ins_LD_A_E() {reg.A = reg.E;}

void ins_LD_A_H() {reg.A = reg.H;}

void ins_LD_A_L() {reg.A = reg.L;}

/*  Load register value into reg B */

void ins_LD_B_A() {reg.B = reg.A;}

void ins_LD_B_B() {reg.B = reg.B;}

void ins_LD_B_C() {reg.B = reg.C;}

void ins_LD_B_D() {reg.B = reg.D;}

void ins_LD_B_E() {reg.B = reg.E;}

void ins_LD_B_H() {reg.B = reg.H;}

void ins_LD_B_L() {reg.B = reg.L;}

/*  Load register value into reg C */

void ins_LD_C_A() {reg.C = reg.A;}

void ins_LD_C_B() {reg.C = reg.B;}

void ins_LD_C_C() {reg.C = reg.C;}

void ins_LD_C_D() {reg.C = reg.D;}

void ins_LD_C_E() {reg.C = reg.E;}

void ins_LD_C_H() {reg.C = reg.H;}

void ins_LD_C_L() {reg.C = reg.L;}


/* Load register value into reg D */

void ins_LD_D_A() {reg.D = reg.A;}

void ins_LD_D_B() {reg.D = reg.B;}

void ins_LD_D_C() {reg.D = reg.C;}

void ins_LD_D_D() {reg.D = reg.D;}

void ins_LD_D_E() {reg.D = reg.E;}

void ins_LD_D_H() {reg.D = reg.H;}

void ins_LD_D_L() {reg.D = reg.L;}


/*  Load register value into reg E */

void ins_LD_E_A() {reg.E = reg.A;}

void ins_LD_E_B() {reg.E = reg.B;}

void ins_LD_E_C() {reg.E = reg.C;}

void ins_LD_E_D() {reg.E = reg.D;}

void ins_LD_E_E() {reg.E = reg.E;}

void ins_LD_E_H() {reg.E = reg.H;}

void ins_LD_E_L() {reg.E = reg.L;}


/*  Load register value into reg H */

void ins_LD_H_A() {reg.H = reg.A;}

void ins_LD_H_B() {reg.H = reg.B;}

void ins_LD_H_C() {reg.H = reg.C;}

void ins_LD_H_D() {reg.H = reg.D;}

void ins_LD_H_E() {reg.H = reg.E;}

void ins_LD_H_H() {reg.H = reg.H;}

void ins_LD_H_L() {reg.H = reg.L;}


/*  Load register value into reg L */

void ins_LD_L_A() {reg.L = reg.A;}

void ins_LD_L_B() {reg.L = reg.B;}

void ins_LD_L_C() {reg.L = reg.C;}

void ins_LD_L_D() {reg.L = reg.D;}

void ins_LD_L_E() {reg.L = reg.E;}

void ins_LD_L_H() {reg.L = reg.H;}

void ins_LD_L_L() {reg.L = reg.L;}




/* 1 word 2 M cycles cycles
 * load value into register from address at reg HL */
void ins_LD_A_memHL() {reg.A = mem[reg.HL];}

void ins_LD_B_memHL() {reg.B = mem[reg.HL];}

void ins_LD_C_memHL() {reg.C = mem[reg.HL];}

void ins_LD_D_memHL() {reg.D = mem[reg.HL];}

void ins_LD_E_memHL() {reg.E = mem[reg.HL];}

void ins_LD_H_memHL() {reg.H = mem[reg.HL];}

void ins_LD_L_memHL() {reg.L = mem[reg.HL];}



/* 1 word 2 M cycles 
 * Load value from register r to mem location HL */
void ins_LD_memHL_A() {reg.A = mem[reg.HL];}

void ins_LD_memHL_B() {reg.B = mem[reg.HL];}

void ins_LD_memHL_C() {reg.C = mem[reg.HL];}

void ins_LD_memHL_D() {reg.D = mem[reg.HL];}

void ins_LD_memHL_E() {reg.E = mem[reg.HL];}

void ins_LD_memHL_H() {reg.H = mem[reg.HL];}

void ins_LD_memHL_L() {reg.L = mem[reg.HL];}



/*  2 words 3 M cycles 
 *  Load value from 2nd word into mem location HL */
void ins_LD_HL_n() { mem[reg.HL] = mem[IMMEDIATE_8_BIT];}


/*  1 Word 2 M Cyles */
void ins_LD_A_memBC() { reg.A = mem[reg.BC]; }

/* 1 Word 2 M Cycles */
void ins_LD_A_memDE() { reg.A = mem[reg.DE]; }

/*  3 Words 4 M Cycles*/
/* Load value stored at mem location 2nd byte and 3rd byte
 * into A  */
void ins_LD_A_memnn() { reg.A = mem[IMMEDIATE_16_BIT]; }

/*  2 Words 2 M Cycles */
void ins_LD_memBC_A() { mem[reg.BC] = reg.A; }


void ins_LD_memDE_A() { mem[reg.DE] = reg.A; }


void ins_LD_memnn_A() { mem[IMMEDIATE_16_BIT] = (reg.A); }

/*  Put value at address HL into A
 *  decrement HL 1 word 2 m cycles*/
void ins_LDD_A_HL() { reg.A = mem[reg.HL]; reg.HL--; }

/*  Put A into memory address HL 
 *  decrement HL 1 word 2 m cycles*/
void ins_LDD_HL_A() { mem[reg.HL] = reg.A; reg.HL--; }

/* Put value at address HL into A,
 * increment HL 1 word 2 m cycles*/
void ins_LDI_A_HL() { reg.A = mem[reg.HL]; reg.HL++; }

/* Put A into memory address HL
 * increment HL 1 word 2 m cycles */
void ins_LDI_HL_A() { mem[reg.HL] = reg.A; reg.HL++; }


/* Put A into memory address $FF00+n*/
/* 2 Words 3 m Cycles */
void ins_LDH_n_A() { mem[0xFF00 + IMMEDIATE_8_BIT] = reg.A; }

/*  Put memory address $FF00+n into A */
/*  2 Words 2 m cycles*/
void ins_LDH_A_n_() { reg.A = mem[0xFF00 + IMMEDIATE_8_BIT]; }



/******************* 16 bit load operations ****************/


/*  Load 16 bit immediate value into combined reg */
void ins_LD_16_IM(uint16_t *r){*r = IMMEDIATE_16_BIT;}

void ins_LD_BC_IM() {ins_LD_16_IM(&reg.BC);}

void ins_LD_DE_IM() {ins_LD_16_IM(&reg.DE);}

void ins_LD_HL_IM() {ins_LD_16_IM(&reg.HL);}

void ins_LD_SP_IM() {ins_LD_16_IM(&reg.SP);}


/*  Load HL into stack pointer */
void LD_SP_HL() {reg.SP = reg.HL;}

/*  Place SP + Immediate 8 bit into HL */
void LD_SP_HL_n() {reg.HL =reg.SP + IMMEDIATE_8_BIT;}


void to_mem(uint16_t value, uint16_t mem_loc) {mem[mem_loc+1] = value >> 8; mem[mem_loc] = value & 0xFF;}

void from_mem(uint16_t *value, uint16_t mem_loc) {*value = mem[mem_loc]; *value |= (mem[mem_loc+1] << 8);}

/*  SP place into memory at immediate address nn */
void LD_nn_SP() {to_mem(reg.SP, IMMEDIATE_16_BIT);}


/*  Push register pair onto stack, decrement stack pointer */
void PUSH(uint16_t r) {to_mem(r, reg.SP); reg.SP-=2; }

void PUSH_AF() {PUSH(reg.AF);}

void PUSH_BC() {PUSH(reg.BC);}

void PUSH_DE() {PUSH(reg.DE);}

void PUSH_HL() {PUSH(reg.HL);}


/*  Pop register pair from stack, increment stack pointer */
void POP(uint16_t *r) {from_mem(r, reg.SP); reg.SP+=2;}

void POP_AF() {POP(&(reg.AF));}

void POP_BC() {POP(&(reg.BC));}

void POP_DE() {POP(&(reg.DE));}

void POP_HL() {POP(&(reg.HL));}

/**********************  8 bit ALU *****************/

/* Reset flags, add value2 to value1, set appropriate flags */
inline uint8_t ADD_8(uint8_t val1, uint8_t val2) 
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
void ADD_A_memHL(){reg.A = ADD_8(reg.A, mem[reg.HL]);}
void ADD_A_Im8(){reg.A = ADD_8(reg.A, IMMEDIATE_8_BIT);}    

inline uint8_t ADC_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = (val1 & 0xF) + (val2 & 0xF) + reg.C_FLAG > 0xF ? 1 : 0;
    val1 += val2 + reg.C_FLAG;
    reg.C_FLAG = val1 < val2 || reg.C_FLAG == 1 && val1 == val2 ? 1 : 0;
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
void ADC_A_memHL(){reg.A = ADC_8(reg.A, mem[reg.HL]);}
void ADC_A_Im8(){reg.A = ADC_8(reg.A, IMMEDIATE_8_BIT);}    


inline uint8_t SUB_8(uint8_t val1, uint8_t val2)
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
void SUB_A_memHL(){reg.A = SUB_8(reg.A, mem[reg.HL]);}
void SUB_A_Im8(){reg.A = SUB_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs SUB carry operation on 2 bytes, returns result and sets flags */
inline uint8_t SBC_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 1;
    reg.Z_FLAG = val1 == val2 + reg.C_FLAG ? 1 : 0;
    reg.H_FLAG = (val1 & 0xF) < (val2 & 0xF) + reg.C_FLAG ? 1 : 0;
    reg.C_FLAG = val1 < val2 || reg.C_FLAG == 1 && val1 == val2 ? 1 : 0;
    return val1 - val2;
}


/* Performs AND operation on 2 bytes, returns result and sets flags */
inline uint8_t AND_8(uint8_t val1, uint8_t val2)
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
void AND_A_memHL(){reg.A = AND_8(reg.A, mem[reg.HL]);}
void AND_A_Im8(){reg.A = AND_8(reg.A, IMMEDIATE_8_BIT);}  



/*  Performs OR operation on 2 bytes, returns result and sets flags */

inline uint8_t OR_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = 0;
    reg.C_FLAG = 0;
    val1 = val1 | val2;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;
}

void OR_A_A(){reg.A = OR_8(reg.A, reg.A);}
void OR_A_B(){reg.A = OR_8(reg.A, reg.B);}
void OR_A_C(){reg.A = OR_8(reg.A, reg.C);}
void OR_A_D(){reg.A = OR_8(reg.A, reg.D);} 
void OR_A_E(){reg.A = OR_8(reg.A, reg.E);} 
void OR_A_H(){reg.A = OR_8(reg.A, reg.H);} 
void OR_A_L(){reg.A = OR_8(reg.A, reg.L);} 
void OR_A_memHL(){reg.A = OR_8(reg.A, mem[reg.HL]);}
void OR_A_Im8(){reg.A = OR_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs XOR operation on 2 bytes, returns result and sets flags */

inline uint8_t XOR_8(uint8_t val1, uint8_t val2) 
{
    reg.N_FLAG = 0;
    reg.H_FLAG = 0;
    reg.C_FLAG = 0;
    val1 = val1 ^ val2;
    reg.Z_FLAG = val1 == 0 ? 1 : 0;

}

void XOR_A_A(){reg.A = XOR_8(reg.A, reg.A);}
void XOR_A_B(){reg.A = XOR_8(reg.A, reg.B);}
void XOR_A_C(){reg.A = XOR_8(reg.A, reg.C);}
void XOR_A_D(){reg.A = XOR_8(reg.A, reg.D);} 
void XOR_A_E(){reg.A = XOR_8(reg.A, reg.E);} 
void XOR_A_H(){reg.A = XOR_8(reg.A, reg.H);} 
void XOR_A_L(){reg.A = XOR_8(reg.A, reg.L);} 
void XOR_A_memHL(){reg.A = XOR_8(reg.A, mem[reg.HL]);}
void XOR_A_Im8(){reg.A = XOR_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs Compare operation on 2 bytes, sets flags */

inline void CP_8(uint8_t val1, uint8_t val2)
{
    reg.N_FLAG = 0;
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
void CP_A_memHL(){ CP_8(reg.A, mem[reg.HL]);}
void CP_A_Im8(){CP_8(reg.A, IMMEDIATE_8_BIT);}  


/*  Performs Increment operation on register, sets flags */
inline uint8_t INC_8(uint8_t val)
{
    val++; 
    reg.N_FLAG = 0;
    reg.H_FLAG = val & 0xF == 0 ? 1 : 0;
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
void INC_memHL(){mem[reg.HL] = INC_8(mem[reg.HL]);}


/*  Performs Decrement operation on register, sets flags */
inline uint8_t DEC_8(uint8_t val)
{
    val--;
    reg.N_FLAG = 1;
    reg.H_FLAG = val & 0xF == 0xF ? 1 : 0;
    reg.Z_FLAG = !val;
}


void DEC_A(){reg.A = DEC_8(reg.A);}
void DEC_B(){reg.B = DEC_8(reg.B);}
void DEC_C(){reg.C = DEC_8(reg.C);}
void DEC_D(){reg.D = DEC_8(reg.D);} 
void DEC_E(){reg.E = DEC_8(reg.E);} 
void DEC_H(){reg.H = DEC_8(reg.H);} 
void DEC_L(){reg.L = DEC_8(reg.L);} 
void DEC_memHL(){mem[reg.HL] = DEC_8(mem[reg.HL]);}


/**********************  16 bit ALU *****************/

/*  Performs Add for 2 16bit values, sets flags */
inline uint16_t ADD_16(uint16_t val1, uint16_t val2)
{
    reg.N_FLAG = 0;
    reg.H_FLAG = (val1 & 0x0FFF) + (val2 & 0x0FFF) > 0x0FFF ? 1 : 0;
    reg.C_FLAG = 0xFFFF - val1 < val2 ? 1 : 0;
    return val1 + val2;
}

void ADD_HL_BC() {reg.HL = ADD_16(reg.HL, reg.BC);}
void ADD_HL_DE() {reg.HL = ADD_16(reg.HL, reg.DE);}
void ADD_HL_HL() {reg.HL = ADD_16(reg.HL, reg.HL);}
void ADD_HL_SP() {reg.HL = ADD_16(reg.HL, reg.SP);}





