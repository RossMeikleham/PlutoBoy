/* Z80 GameBoy CPU*/
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


//8 Bit Load Operations

/* Load 8 bit immediate value into specified location */
void ins_LD_8_IM(uint8_t *loc) { *loc = mem[IMMEDIATE_8_BIT];}

void ins_LD_A_n(){ins_LD_n(&reg.A);}

void ins_LD_B_n(){ins_LD_n(&reg.B);}

void ins_LD_C_n(){ins_LD_n(&reg.C);}

void ins_LD_D_n(){ins_LD_n(&reg.D);}

void ins_LD_E_n(){ins_LD_n(&reg.E);}

void ins_LD_H_n(){ins_LD_n(&reg.H);}

void ins_LD_L_n(){ins_LD_n(&reg.L);}



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


//16 bit load operations




