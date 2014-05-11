/* Modified Z80 GameBoy CPU*/
/* Ross Meikleham */
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "cpu.h"

#define IMMEDIATE_8_BIT get_mem(reg.PC+1)
#define IMMEDIATE_16_BIT (get_mem((reg.PC)+2)<< 8) | get_mem((reg.PC+1))

static uint8_t opcode;
static int halt = 0;
static int stop = 0;
static int interrupts_disabled = 0;


union REGISTERS

{
  struct{uint16_t AF,BC,DE,HL,SP,PC;};
  
  struct{uint8_t F,A,C,B,E,D,L,H,SPlo,SPhi,PClo,PChi;};
  struct{uint8_t UNUSED_FLAG4:1, UNUSED_FLAG3:1, UNUSED_FLAG2:1, 
  UNUSED_FLAG:1, C_FLAG:1, H_FLAG:1, N_FLAG:1, Z_FLAG:1;};

} static reg;



#include "opcodes.c"  //Include opcode implementations


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
    int const * words; /*  No of words per instruction */
    Instruction const * const ext_instruction_set; /* extended 0xCB instructions */

    /*  Separate the names from cycle/pointer information
     *  for closer cache locaility in performance. These are
     *  only used for dissasembly information. */
    char const * const *asm_name_set_1;
    char const * const *asm_name_set_2; 

} Instructions;


static Instruction ins[UINT8_MAX+1] = {
    
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

static Instruction ext_ins[UINT8_MAX+1] = {

    {8, RLC_B}, {8, RLC_C}, {8, RLC_D},      {8, RLC_E}, 
    {8, RLC_H}, {8, RLC_L}, {16, RLC_memHL}, {8, RLC_A}, 
    {8, RRC_B}, {8, RRC_C}, {8, RRC_D},      {8, RRC_E},
    {8, RRC_H}, {8, RRC_L}, {16, RRC_memHL}, {8, RRC_A},


    {8, RL_B}, {8, RL_C}, {8, RL_D},      {8, RL_E},
    {8, RL_H}, {8, RL_L}, {16, RL_memHL}, {8, RL_A}, 
    {8, RR_B}, {8, RR_C}, {8, RR_D},      {8, RR_E},
    {8, RR_H}, {8, RR_L}, {16, RR_memHL}, {8, RR_A},

    {8, SLA_B}, {8, SLA_C}, {8, SLA_D},      {8, SLA_E},
    {8, SLA_H}, {8, SLA_L}, {16, SLA_memHL}, {8, SLA_A}, 
    {8, SRA_B}, {8, SRA_C}, {8, SRA_D},      {8, SRA_E},
    {8, SRA_H}, {8, SRA_L}, {16, SRA_memHL}, {8, SRA_A},

    {8, SWAP_B}, {8, SWAP_C}, {8, SWAP_D},      {8, SWAP_E},
    {8, SWAP_H}, {8, SWAP_L}, {16, SWAP_memHL}, {8, SWAP_A}, 
    {8, SRL_B},  {8, SRL_C},  {8, SRL_D},       {8, SRL_E},
    {8, SRL_H},  {8, SRL_L},  {16, SRL_memHL},  {8, SRL_A},

    {8, BIT_B_0}, {8, BIT_C_0}, {8, BIT_D_0},      {8, BIT_E_0},
    {8, BIT_H_0}, {8, BIT_L_0}, {12, BIT_memHL_0}, {8, BIT_A_0},
    {8, BIT_B_1}, {8, BIT_C_1}, {8, BIT_D_1},      {8, BIT_E_1}, 
    {8, BIT_H_1}, {8, BIT_L_1}, {12, BIT_memHL_1}, {8, BIT_A_1},
     
    {8, BIT_B_2}, {8, BIT_C_2}, {8, BIT_D_2},      {8, BIT_E_2}, 
    {8, BIT_H_2}, {8, BIT_L_2}, {12, BIT_memHL_2}, {8, BIT_A_2}, 
    {8, BIT_B_3}, {8, BIT_C_3}, {8, BIT_D_3},      {8, BIT_E_3}, 
    {8, BIT_H_3}, {8, BIT_L_3}, {12, BIT_memHL_3}, {8, BIT_A_3},

    {8, BIT_B_4}, {8, BIT_C_4}, {8, BIT_D_4},      {8, BIT_E_4}, 
    {8, BIT_H_4}, {8, BIT_L_4}, {12, BIT_memHL_4}, {8, BIT_A_4},
    {8, BIT_B_5}, {8, BIT_C_5}, {8, BIT_D_5},      {8, BIT_E_5}, 
    {8, BIT_H_5}, {8, BIT_L_5}, {12, BIT_memHL_5}, {8, BIT_A_5},
     
    {8, BIT_B_6}, {8, BIT_C_6}, {8, BIT_D_6},      {8, BIT_E_6}, 
    {8, BIT_H_6}, {8, BIT_L_6}, {12, BIT_memHL_6}, {8, BIT_A_6}, 
    {8, BIT_B_7}, {8, BIT_C_7}, {8, BIT_D_7},      {8, BIT_E_7},
    {8, BIT_H_7}, {8, BIT_L_7}, {12, BIT_memHL_7}, {8, BIT_A_7},
    
    
    {8, RES_B_0}, {8, RES_C_0}, {8, RES_D_0},      {8, RES_E_0}, 
    {8, RES_H_0}, {8, RES_L_0}, {16, RES_memHL_0}, {8, RES_A_0},
    {8, RES_B_1}, {8, RES_C_1}, {8, RES_D_1},      {8, RES_E_1}, 
    {8, RES_H_1}, {8, RES_L_1}, {16, RES_memHL_1}, {8, RES_A_1},
     
    {8, RES_B_2}, {8, RES_C_2}, {8, RES_D_2},      {8, RES_E_2}, 
    {8, RES_H_2}, {8, RES_L_2}, {16, RES_memHL_2}, {8, RES_A_2}, 
    {8, RES_B_3}, {8, RES_C_3}, {8, RES_D_3},      {8, RES_E_3}, 
    {8, RES_H_3}, {8, RES_L_3}, {16, RES_memHL_3}, {8, RES_A_3},

    {8, RES_B_4}, {8, RES_C_4}, {8, RES_D_4},      {8, RES_E_4}, 
    {8, RES_H_4}, {8, RES_L_4}, {16, RES_memHL_4}, {8, RES_A_4},
    {8, RES_B_5}, {8, RES_C_5}, {8, RES_D_5},      {8, RES_E_5}, 
    {8, RES_H_5}, {8, RES_L_5}, {16, RES_memHL_5}, {8, RES_A_5},
     
    {8, RES_B_6}, {8, RES_C_6}, {8, RES_D_6},      {8, RES_E_6},
    {8, RES_H_6}, {8, RES_L_6}, {16, RES_memHL_6}, {8, RES_A_6}, 
    {8, RES_B_7}, {8, RES_C_7}, {8, RES_D_7},      {8, RES_E_7}, 
    {8, RES_H_7}, {8, RES_L_7}, {16, RES_memHL_7}, {8, RES_A_7},      
    
    {8, SET_B_0}, {8, SET_C_0}, {8, SET_D_0},      {8, SET_E_0}, 
    {8, SET_H_0}, {8, SET_L_0}, {16, SET_memHL_0}, {8, SET_A_0},
    {8, SET_B_1}, {8, SET_C_1}, {8, SET_D_1},      {8, SET_E_1}, 
    {8, SET_H_1}, {8, SET_L_1}, {16, SET_memHL_1}, {8, SET_A_1}, 

    {8, SET_B_2}, {8, SET_C_2}, {8, SET_D_2},      {8, SET_E_2}, 
    {8, SET_H_2}, {8, SET_L_2}, {16, SET_memHL_2}, {8, SET_A_2}, 
    {8, SET_B_3}, {8, SET_C_3}, {8, SET_D_3},      {8, SET_E_3}, 
    {8, SET_H_3}, {8, SET_L_3}, {16, SET_memHL_3}, {8, SET_A_3},

    {8, SET_B_4}, {8, SET_C_4}, {8, SET_D_4},      {8, SET_E_4}, 
    {8, SET_H_4}, {8, SET_L_4}, {16, SET_memHL_4}, {8, SET_A_4},
    {8, SET_B_5}, {8, SET_C_5}, {8, SET_D_5},      {8, SET_E_5}, 
    {8, SET_H_5}, {8, SET_L_5}, {16, SET_memHL_5}, {8, SET_A_5},
     
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
    2,1,2,1,1,1,2,1,2,1,3,1,1,1,2,1,
    2,1,2,1,1,1,2,1,2,1,3,1,1,1,2,1
};

static char const * const asm_instruction_set[UINT8_MAX+1] = { 
    "NOP", "LD BC,d16", "LD (BC),A", "INC BC", 
    "INC B","DEC B", "LD B,d8", "RLCA", 
    "LD (a16),SP", "ADD HL,BC", "LD A,(BC)", "DEC BC", 
    "INC C","DEC C", "LD C,d8", "RRCA",
    
    "STOP 0", "LD DE,d16", "LD (DE),A", "INC DE",
    "INC D", "DEC D", "LD D,d8", "RLA",
    "JR r8", "ADD HL,DE", "LD A,(DE)", "DEC DE",
    "INC E", "DEC E", "LD E,de", "RRA",
     
    "JR NZ,r8", "LD HL,d16", "LD (HL+),A", "INC HL",
    "INC H", "DEC H", "LD H,d8", "DAA",
    "JR Z,r8", "ADD HL,HL", "LD A,(HL+)", "DEC HL",
    "INC L", "DEC L", "LD L,d8", "CPL",
     
    "JR NC,r8", "LD SP,d16", "LD (HL-),A", "INC SP",
    "INC (HL)", "DEC (HL)", "LD (HL),d8", "SCF",
    "JR C,r8", "ADD HL,SP","LD A,(HL-)", "DEC SP",
    "INC A", "DEC A", "LD A,d8", "CCF",

    "LD B,B", "LD B,C", "LD B,D", "LD B,E", 
    "LD B,H","LD B,L", "LD B,(HL)", "LD B,A"
    "LD C,B", "LD C,C", "LD C,D", "LD C,E",
    "LD C,H", "LD C,L", "LD C,(HL)", "LD C,A", 

    "LD D,B", "LD D,C", "LD D,D", "LD D,E", 
    "LD D,H","LD D,L", "LD D,(HL)", "LD D,A"
    "LD E,B", "LD E,C", "LD E,D", "LD E,E",
    "LD E,H", "LD E,L", "LD E,(HL)", "LD E,A", 
    
    "LD H,B", "LD H,C", "LD H,D", "LD H,E", 
    "LD H,H","LD H,L", "LD H,(HL)", "LD H,A"
    "LD L,B", "LD L,C", "LD L,D", "LD L,E",
    "LD L,H", "LD L,L", "LD L,(HL)", "LD L,A", 
    
    "LD (HL),B", "LD (HL),C", "LD (HL),D", "LD (HL),E", 
    "LD (HL),H","LD (HL),L", "HALT", "LD (HL),A"
    "LD A,B", "LD A,C", "LD A,D", "LD A,E",
    "LD A,H", "LD A,L", "LD A,(HL)", "LD A,A", 

    "ADD A,B", "ADD A,C", "ADD A,D", "ADD A,E",
    "ADD A,H", "ADD A,L", "ADD A,(HL)", "ADD A,A",
    "ADC A,B", "ADC A,C", "ADC A,D", "ADC A,E",
    "ADC A,H", "ADC A,L", "ADC A,(HL)", "ADC A,A",

    "SUB A,B", "SUB A,C", "SUB A,D", "SUB A,E",
    "SUB A,H", "SUB A,L", "SUB A,(HL)", "SUB A,A",
    "SBC A,B", "SBC A,C", "SBC A,D", "SBC A,E",
    "SBC A,H", "SBC A,L", "SBC A,(HL)", "SBC A,A",
 
    "AND A,B", "AND A,C", "AND A,D", "AND A,E",
    "AND A,H", "AND A,L", "AND A,(HL)", "AND A,A",
    "XOR A,B", "XOR A,C", "XOR A,D", "XOR A,E",
    "XOR A,H", "XOR A,L", "XOR A,(HL)", "XOR A,A",

    "OR A,B", "OR A,C", "OR A,D", "OR A,E",
    "OR A,H", "OR A,L", "OR A,(HL)", "OR A,A",
    "CP A,B", "CP A,C", "CP A,D", "CP A,E",
    "CP A,H", "CP A,L", "CP A,(HL)", "CP A,A",

    "RET NZ", "POP BC", "JP NZ,a16", "JP a16",
    "CALL NZ,a16", "PUSH BC", "ADD A,d8", "RST 00H", 
    "RET Z", "RET", "JP Z,a16", "EXT_INSTRUCTION", 
    "CALL Z,a16", "CALL a16", "ADC A,d8", "RST 08H",

    "RET NC", "POP DE", "JP NC,a16", "NONE", 
    "CALL NC,a16", "PUSH DE", "SUB d8", "RST 10H", 
    "RET C", "RETI", "JP C,a16", "NONE", 
    "CALL C,a16", "NONE", "SBC A,d8", "RST 18H",

    "LDH (a8), A", "POP HL", "LD (C),A", "NONE",
    "NONE", "PUSH HL", "AND d8", "RST 20H",
    "ADD SP,r8", "JP (HL)", "LD (a16),A", "NONE",
    "NONE", "NONE", "XOR d8", "RST 28H",
    
    "LDH A,(a8)", "POP AF", "LD A,(C)", "DI",
    "NONE", "PUSH AF", "OR d8", "RST 30H", 
    "LD HL,SP+r8", "LD SP,HL", "LD A,(a16)", "EI",
    "NONE", "NONE", "CP d8", "RST 38H"
    
};

static char const * const asm_ext_instruction_set[UINT8_MAX+1] = {

    "RLC B", "RLC C", "RLC D", "RLC E", 
    "RLC H", "RLD L", "RLC (HL)", "RLC A", 

    "RRC B", "RRC C", "RRC D", "RRC E", 
    "RRC H", "RRC L", "RRC (HL)", "RRC A", 

    "RL B", "RL C", "RL D", "RL E", 
    "RL H", "RL L", "RL (HL)", "RL A", 

    "RR B", "RR C", "RR D", "RR E", 
    "RR H", "RR L", "RR (HL)", "RR A", 

    "SLA B", "SLA C", "SLA D", "SLA E", 
    "SLA H", "SLA L", "SLA (HL)", "SLA A", 

    "SRA B", "SRA C", "SRA D", "SRA E", 
    "SRA H", "SRA L", "SRA (HL)", "SRA A",
     
    "SWAP B", "SWAP C", "SWAP D", "SWAP E", 
    "SWAP H", "SWAP L", "SWAP (HL)", "SWAP A",
    
    "SRL B", "SRL C", "SRL D", "SRL E", 
    "SRL H", "SRL L", "SRL (HL)", "SRL A",
    
    "BIT 0,B", "BIT 0,C", "BIT 0,D", "BIT 0,E", 
    "BIT 0,H", "BIT 0,L", "BIT 0,(HL)", "BIT 0,A"
    "BIT 1,B", "BIT 1,C", "BIT 1,D", "BIT 1,E", 
    "BIT 1,H", "BIT 1,L", "BIT 1,(HL)", "BIT 1,A"      
    "BIT 2,B", "BIT 2,C", "BIT 2,D", "BIT 2,E", 
    "BIT 2,H", "BIT 2,L", "BIT 2,(HL)", "BIT 2,A" 
    "BIT 3,B", "BIT 3,C", "BIT 3,D", "BIT 3,E", 
    "BIT 3,H", "BIT 3,L", "BIT 3,(HL)", "BIT 3,A" 
    "BIT 4,B", "BIT 4,C", "BIT 4,D", "BIT 4,E", 
    "BIT 4,H", "BIT 4,L", "BIT 4,(HL)", "BIT 4,A" 
    "BIT 5,B", "BIT 5,C", "BIT 5,D", "BIT 5,E", 
    "BIT 5,H", "BIT 5,L", "BIT 5,(HL)", "BIT 5,A" 
    "BIT 6,B", "BIT 6,C", "BIT 6,D", "BIT 6,E", 
    "BIT 6,H", "BIT 6,L", "BIT 6,(HL)", "BIT 6,A" 
    "BIT 7,B", "BIT 7,C", "BIT 7,D", "BIT 7,E", 
    "BIT 7,H", "BIT 7,L", "BIT 7,(HL)", "BIT 7,A",
    
    "RES 0,B", "RES 0,C", "RES 0,D", "RES 0,E",
    "RES 0,H", "RES 0,L", "RES 0,(HL)", "RES 0,A",
    "RES 1,B", "RES 1,C", "RES 1,D", "RES 1,E",
    "RES 1,H", "RES 1,L", "RES 1,(HL)", "RES 1,A", 
    "RES 2,B", "RES 2,C", "RES 2,D", "RES 2,E",
    "RES 2,H", "RES 2,L", "RES 2,(HL)", "RES 2,A", 
    "RES 3,B", "RES 3,C", "RES 3,D", "RES 3,E",
    "RES 3,H", "RES 3,L", "RES 3,(HL)", "RES 3,A", 
    "RES 4,B", "RES 4,C", "RES 4,D", "RES 4,E",
    "RES 4,H", "RES 4,L", "RES 4,(HL)", "RES 4,A", 
    "RES 5,B", "RES 5,C", "RES 5,D", "RES 5,E",
    "RES 5,H", "RES 5,L", "RES 5,(HL)", "RES 5,A", 
    "RES 6,B", "RES 6,C", "RES 6,D", "RES 6,E",
    "RES 6,H", "RES 6,L", "RES 6,(HL)", "RES 6,A", 
    "RES 7,B", "RES 7,C", "RES 7,D", "RES 7,E",
    "RES 7,H", "RES 7,L", "RES 7,(HL)", "RES 7,A",

    "SET 0,B", "SET 0,C", "SET 0,D", "SET 0,E",
    "SET 0,H", "SET 0,L", "SET 0,(HL)", "SET 0,A",
    "SET 1,B", "SET 1,C", "SET 1,D", "SET 1,E",
    "SET 1,H", "SET 1,L", "SET 1,(HL)", "SET 1,A",
    "SET 2,B", "SET 2,C", "SET 2,D", "SET 2,E",
    "SET 2,H", "SET 2,L", "SET 2,(HL)", "SET 2,A",
    "SET 3,B", "SET 3,C", "SET 3,D", "SET 3,E",
    "SET 3,H", "SET 3,L", "SET 3,(HL)", "SET 3,A",
    "SET 4,B", "SET 4,C", "SET 4,D", "SET 4,E",
    "SET 4,H", "SET 4,L", "SET 4,(HL)", "SET 4,A",
    "SET 5,B", "SET 5,C", "SET 5,D", "SET 5,E",
    "SET 5,H", "SET 5,L", "SET 5,(HL)", "SET 5,A",
    "SET 6,B", "SET 6,C", "SET 6,D", "SET 6,E",
    "SET 6,H", "SET 6,L", "SET 6,(HL)", "SET 6,A",
    "SET 7,B", "SET 7,C", "SET 7,D", "SET 7,E",
    "SET 7,H", "SET 7,L", "SET 7,(HL)", "SET 7,A",

};
 
static Instructions instructions = {
    .instruction_set = ins, .words = ins_words, 
    .ext_instruction_set = ext_ins, 
    .asm_name_set_1 = asm_instruction_set, 
    .asm_name_set_2 = asm_ext_instruction_set
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
int exec_opcode() {
    
    
    uint8_t opcode;
    int i;
    //printf("pc location:%x\n", reg.PC);
    //printf("reg b %x\n", reg.B);
    for (i = 0xfff9; i < 0x10000; i++) {
      //  printf("mem:%x\n",get_mem(i));
    }
    opcode = get_mem(reg.PC);
    //printf("OPCODE:%X, PC:%X SP:%X A:%X F:%X B:%X C:%X D:%X E:%X\n",opcode,reg.PC,reg.SP,reg.A,reg.F,reg.B,reg.C,reg.D,reg.E);

    if (opcode != 0xCB) {
        //printf("opcode %x\n", opcode);
        instructions.instruction_set[opcode].operation();
        reg.PC++;
        return instructions.instruction_set[opcode].cycles;
    } else {
        /*  extended instruction */
        opcode = get_mem(++reg.PC);
        instructions.ext_instruction_set[opcode].operation();
        reg.PC++;
        //printf("extended opcode %x\n", opcode);
        return instructions.ext_instruction_set[opcode].cycles;
    }
}






