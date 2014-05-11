/* Modified Z80 GameBoy CPU*/
/* Ross Meikleham */
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "cpu.h"


union REGISTERS

{
  struct{uint16_t AF,BC,DE,HL,SP,PC;};
  
  struct{uint8_t F,A,C,B,E,D,L,H,SPlo,SPhi,PClo,PChi;};
  struct{uint8_t UNUSED_FLAG4:1, UNUSED_FLAG3:1, UNUSED_FLAG2:1, 
  UNUSED_FLAG:1, C_FLAG:1, H_FLAG:1, N_FLAG:1, Z_FLAG:1;};

} static reg;

#define IMMEDIATE_8_BIT get_mem(reg.PC+1)
#define IMMEDIATE_16_BIT (get_mem((reg.PC)+2)<< 8) | get_mem((reg.PC+1))

uint8_t opcode;
int halt = 0;
int stop = 0;
int interrupts_disabled = 0;


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


//uint8_t cycles = {};
//(void)()* getOp() 
//{
//}

void print_regs() {
    printf("AF:%x-%x\n",reg.A,reg.F);
    printf("BC:%x-%x\n",reg.B,reg.C);
    printf("DE:%x-%x\n",reg.D,reg.E);
    printf("HL:%x-%x\n",reg.H,reg.L);
    printf("PC:%x\n",reg.PC);
    printf("SP:%x\n",reg.SP);
}
// Pointer to function which performs an operation
// based on an opcode
typedef void (*Operation)(void); 

typedef struct {

    int const cycles; /*  Number of machine cycles per instruction */
    Operation const operation; /* operation which performs the instruction */
    
} Instruction;


/*  Information on all processor instructions 
 *  including extended instructions  */
typedef struct {

    Instruction const * const instruction_set; 
    Instruction const * const ext_instruction_set; /* extended 0xCB instructions */

    /*  Separate the names from cycle/pointer information
     *  for closer cache locaility in performance. These are
     *  only used for dissasembly information. */
    char const * const asm_name_set_1;
    char const * const asm_name_set_2; 

} Instructions;


static Instructions instructions;
instructions.instruction_set_1 = { 

    // 0x00 - 0x0F
    {4, NOP}, {12, LD_BC_IM}, {8, LD_memBC_A}, {8, INC_BC},     
    {4, INC_B}, {4, DEC_B}, {8, LD_B_IM}, {4, RLC_A},     
    {20, LD_nn_SP}, {8, ADD_HL_BC}, {8, LD_A_memBC}, {8, DEC_BC},     
    {4, INC_C}, {4, DEC_C}, {8, LD_C_IM}, {4, RRC_A},
    
    //0x10 - 0x1F
    {8, STOP}, {12, LD_DE_IM}, {8, LD_memDE_A}, {8, INC_DE},     
    {4, INC_D}, {4, DEC_D}, {8, LD_D_IM}, {4, RL_A},
    {12, JR_n}, {8, ADD_HL_DE}, {8, LD_A_memDE}, {8, DEC_DE},
    {4, INC_E}, {4, DEC_E}, {8, LD_E_IM}, {4, RR_A},

    //0x20 - 0x2F
    {8, JR_NZ_n}, {12, LD_HL_IM}, {8, LDI_HL_A}, {8, INC_HL},
    {4, INC_H}, {4, DEC_H}, {8, LD_H_IM}, {4, DAA},
    {8, JR_Z_n}, {8, ADD_HL_HL}, {8, LDI_A_HL}, {8, DEC_HL},
    {4, INC_L}, {4, DEC_L}, {8, LD_L_IM}, {4,CPL},

    //0x30 - 0x3F
    {8, JR_NC_n}, {12, LD_SP_IM}, {8, LDD_HL_A}, {8, INC_SP},
    {12, INC_memHL}, {12, DEC_memHL}, {12, LD_HL_n}, {4, SCF},
    {8, JR_C_n}, {8, ADD_HL_SP}, {8, LDD_A_HL}, {8, DEC_SP},
    {4, INC_A}, {4, DEC_A}, {8, LD_A_IM}, {4, CCF},

    //0x40 - 0x4F
    {4, LD_B_B}, {4, LD_B_C}, {4, LD_B_D}, {4, LD_B_E},     
    {4 ,LD_B_H, {4, LD_B_L}, {8, LD_B_memHL}, {4 ,LD_B_A},
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
    {4, LD_A_B}, {4, LD_A_C}}, {4, LD_A_D}, {4. LD_A_E},
    {4, LD_A_H, {4, LD_A_L}, {8, LD_A_memHL}, {4, LD_A_A},

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
    {4, CP_A_B}, {4, CP_A_C}, {4, CP_A_D}, {4, CP_A_E,}  
    {4, CP_A_H}, {4, CP_A_L}, {8, CP_A_memHL,}, {4, CP_A_A},

    //0xC0 - 0xCF
    {8, RET_NZ}, {12, POP_BC}, {12, JP_NZ_nn}, {16, JP_nn},
    {12, CALL_NZ_nn}, {16, PUSH_BC}, {8, ADD_A_Im8}, {16, RST_00},
    {8, RET_Z}, {16, RET}, {12, JP_Z_nn}, {0, NULL},
    {12, CALL_Z_nn}, {24, CALL_nn}, {8, ADC_A_Im8}, {16, RST_08},

    //0xD0 - 0xDF
    {8, RET_NC}, {12, POP_DE}, {12, JP_NC_nn}, {0, NULL},
    {12, CALL_NC_nn}, {16, PUSH_DE}, {8, SUB_A_Im8}, {16, RST_10},
    {8, RET_C}, {8, RETI}, {16, JP_C_nn}, {0,  NULL}, 
    {12, CALL_C_nn}, {0,  NULL}, {8,  SBC_A_Im8}, {16 ,RST_18},

    //0xE0 - 0xEF
    {12, LDH_n_A}, {12, POP_HL}, {8, LDH_C_A}, {0, NULL},       
    {0, NULL}, {16, PUSH_HL}, {8, AND_A_Im8},{16, RST_20},
    {16, ADD_SP_IM8}, {4, JP_memHL}, {16, LD_memnn_A}, {0, NULL},       
    {0, NULL}, {0, NULL}, {8, XOR_A_Im8}, {16, RST_28},
    
    //0xF0 - 0xFF                                                              
    {12, LDH_A_n}, {12, POP_AF}, {8, LDH_A_C}, {4 ,DI},
    {0,NULL}, {16, PUSH_AF}, {8, OR_A_Im8}, {16, RST_30},
    {12, LD_HL_SP_n}, {8, LD_SP_HL}, {16, LD_A_IM}, {4, EI},
    {0 , NULL}, {0, NULL}, {8, CP_A_Im8}, {16, RST_38}
};





instruction.ext_instruction_set = {
    
    {, RLC_B}, {, RLC_C}, {, RLC_D}, {, RLC_E}, 
    {, RLC_H}, {, RLC_L}, {, RLC_memHL}, {, RLC_A}, 
    {, RRC_B}, {, RRC_C}, {, RRC_D}, {, RRC_E},
    {, RRC_H},{, RRC_L}, {,RRC_memHL}, {,RRC_A},


    {, RL_B}, {, RL_C}, {, RL_D} {, RL_E},
    {, RL_H}, {, RL_L}, {, RL_memHL}, {, RL_A}, 
    {, RR_B}, {, RR_C}, {, RR_D}, {, RR_E},
    {, RR_H}, {, RR_L}, {, RR_memHL}, {, RR_A},

    {, SLA_B}, {, SLA_C}, {,SLA_D}, {,SLA_E},
    {, SLA_H}, {, SLA_L}, {,SLA_memHL}, {,SLA_A}, 
    {, SRA_B}, {, SRA_C},{, SRA_D}, {, SRA_E},
    {, SRA_H}, {, SRA_L}, SRA_memHL, SRA_A,
    SWAP_B, SWAP_C, SWAP_D, SWAP_E, SWAP_H, SWAP_L, SWAP_memHL, SWAP_A, 
    SRL_B, SRL_C, SRL_D, SRL_E, SRL_H, SRL_L, SRL_memHL, SRL_A,

    BIT_B_0, BIT_C_0, BIT_D_0, BIT_E_0, BIT_H_0, BIT_L_0, BIT_memHL_0, BIT_A_0,
    BIT_B_1, BIT_C_1, BIT_D_1, BIT_E_1, BIT_H_1, BIT_L_1, BIT_memHL_1, BIT_A_1, 
    BIT_B_2, BIT_C_2, BIT_D_2, BIT_E_2, BIT_H_2, BIT_L_2, BIT_memHL_2, BIT_A_2, 
    BIT_B_3, BIT_C_3, BIT_D_3, BIT_E_3, BIT_H_3, BIT_L_3, BIT_memHL_3, BIT_A_3,
    BIT_B_4, BIT_C_4, BIT_D_4, BIT_E_4, BIT_H_4, BIT_L_4, BIT_memHL_4, BIT_A_4,
    BIT_B_5, BIT_C_5, BIT_D_5, BIT_E_5, BIT_H_5, BIT_L_5, BIT_memHL_5, BIT_A_5, 
    BIT_B_6, BIT_C_6, BIT_D_6, BIT_E_6, BIT_H_6, BIT_L_6, BIT_memHL_6, BIT_A_6, 
    BIT_B_7, BIT_C_7, BIT_D_7, BIT_E_7, BIT_H_7, BIT_L_7, BIT_memHL_7, BIT_A_7,
    
    
    RES_B_0, RES_C_0, RES_D_0, RES_E_0, RES_H_0, RES_L_0, RES_memHL_0, RES_A_0,
    RES_B_1, RES_C_1, RES_D_1, RES_E_1, RES_H_1, RES_L_1, RES_memHL_1, RES_A_1, 
    RES_B_2, RES_C_2, RES_D_2, RES_E_2, RES_H_2, RES_L_2, RES_memHL_2, RES_A_2, 
    RES_B_3, RES_C_3, RES_D_3, RES_E_3, RES_H_3, RES_L_3, RES_memHL_3, RES_A_3,
    RES_B_4, RES_C_4, RES_D_4, RES_E_4, RES_H_4, RES_L_4, RES_memHL_4, RES_A_4,
    RES_B_5, RES_C_5, RES_D_5, RES_E_5, RES_H_5, RES_L_5, RES_memHL_5, RES_A_5, 
    RES_B_6, RES_C_6, RES_D_6, RES_E_6, RES_H_6, RES_L_6, RES_memHL_6, RES_A_6, 
    RES_B_7, RES_C_7, RES_D_7, RES_E_7, RES_H_7, RES_L_7, RES_memHL_7, RES_A_7,   
    
    
    SET_B_0, SET_C_0, SET_D_0, SET_E_0, SET_H_0, SET_L_0, SET_memHL_0, SET_A_0,
    SET_B_1, SET_C_1, SET_D_1, SET_E_1, SET_H_1, SET_L_1, SET_memHL_1, SET_A_1, 
    SET_B_2, SET_C_2, SET_D_2, SET_E_2, SET_H_2, SET_L_2, SET_memHL_2, SET_A_2, 
    SET_B_3, SET_C_3, SET_D_3, SET_E_3, SET_H_3, SET_L_3, SET_memHL_3, SET_A_3,
    SET_B_4, SET_C_4, SET_D_4, SET_E_4, SET_H_4, SET_L_4, SET_memHL_4, SET_A_4,
    SET_B_5, SET_C_5, SET_D_5, SET_E_5, SET_H_5, SET_L_5, SET_memHL_5, SET_A_5, 
    SET_B_6, SET_C_6, SET_D_6, SET_E_6, SET_H_6, SET_L_6, SET_memHL_6, SET_A_6, 
    SET_B_7, SET_C_7, SET_D_7, SET_E_7, SET_H_7, SET_L_7, SET_memHL_7, SET_A_7};   
   


/*  Table of machine cycles per extended (CB) instruction */
int ext_ins_cycles[] = 
{
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
    8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
    8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
    8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,
    8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8
};

void reset_cpu() {
    /*  Default starting values for normal GB */
    reg.AF = 0x11B0;
    reg.BC = 0x0013;
    reg.DE = 0x00D8;
    reg.HL = 0x014D;
    reg.PC = 0x100;
    reg.SP = 0xFFFE;
}

/*  Executes the next processor instruction and returns
 *  the amount of cycles the instruction takes */
int exec_opcode() {
    
    
    uint8_t opcode;
    int i;
    //printf("pc location:%x\n", reg.PC);
    //printf("reg b %x\n", reg.B);
    for (i = 0xfff9; i < 0x10000; i++) {
      //  printf("mem:%x\n",get_mem(i));
    }
    opcode = get_mem(reg.PC);
    printf("OPCODE:%X, PC:%X SP:%X A:%X F:%X B:%X C:%X D:%X E:%X\n",opcode,reg.PC,reg.SP,reg.A,reg.F,reg.B,reg.C,reg.D,reg.E,reg.SP);

    if (opcode != 0xCB) {
        //printf("opcode %x\n", opcode);
        ins[opcode]();
        reg.PC++;
        return ins_cycles[opcode];
    } else {
        /*  extended instruction */
        opcode = get_mem(++reg.PC);
        ins[opcode]();
        reg.PC++;
        //printf("extended opcode %x\n", opcode);
        return ext_ins_cycles[opcode];
    }
}






