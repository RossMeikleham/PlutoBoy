
#include "Z80.h"

typedef void (*instruction)(void);
instruction ins[] = {NOP, LD_BC_IM, LD_memBC_A, INC_BC, INC_B, DEC_B, LD_B_IM, RLC_A, /*0x00 - 0x07  */
                     LD_nn_SP, ADD_HL_BC, LD_A_memBC, DEC_BC, INC_C, DEC_C, LD_C_IM, RRC_A, /* 0x08- 0x0F*/
                     STOP, LD_DE_IM, LD_memDE_A, INC_DE, INC_D, DEC_D, LD_D_IM, RL_A, /* 0x10 - 0x17 */         
                     JR_n, ADD_HL_DE, LD_A_memDE, DEC_DE, INC_E, DEC_E, LD_E_IM, RR_A, /*0x18 -0x1F */
                     JR_NZ_n, LD_HL_IM, LDI_HL_A, INC_HL, INC_H, DEC_H, LD_H_IM, DAA, /*0x20-0x27*/
                     JR_Z_n, ADD_HL_HL, LDI_A_HL, DEC_HL, INC_L, DEC_L, LD_L_IM, CPL, /* 0x28-0x2F */
                     JR_NC_n, LD_SP_IM, LDD_HL_A, INC_SP, INC_memHL, DEC_memHL, LD_HL_n,SCF,
                     JR_C_n, ADD_HL_SP, LDD_A_HL, DEC_SP, INC_A, DEC_A, LD_A_IM, CCF,
                     LD_B_B, LD_B_C, LD_B_D, LD_B_E, LD_B_H, LD_B_L, LD_B_memHL, LD_B_A,
                     LD_C_B, LD_C_C, LD_C_D, LD_C_E, LD_C_H, LD_C_L, LD_C_memHL, LD_C_A,
                     LD_D_B, LD_D_C, LD_D_D, LD_D_E, LD_D_H, LD_D_L, LD_D_memHL, LD_D_A,
                     LD_E_B, LD_E_C, LD_E_D, LD_E_E, LD_E_H, LD_E_L, LD_E_memHL, LD_E_A,
                     LD_H_B, LD_H_C, LD_H_D, LD_H_E, LD_H_H, LD_H_L, LD_H_memHL, LD_H_A,
                     LD_L_B, LD_L_C, LD_L_D, LD_L_E, LD_L_H, LD_L_L, LD_L_memHL, LD_L_A,
                     LD_memHL_B, LD_memHL_C, LD_memHL_D, LD_memHL_E, LD_memHL_H, LD_memHL_L, HALT, LD_memHL_A,
                     LD_A_B, LD_A_C, LD_A_D, LD_A_E, LD_A_H, LD_A_L, LD_A_memHL, LD_A_A,
                     ADD_A_B, ADD_A_C, ADD_A_D, ADD_A_E, ADD_A_H, ADD_A_L, ADD_A_memHL, ADD_A_A,
                     ADC_A_B, ADC_A_C, ADC_A_D, ADC_A_E, ADC_A_H, ADC_A_L, ADC_A_memHL, ADC_A_A,
                     SUB_A_B, SUB_A_C, SUB_A_D, SUB_A_E, SUB_A_H, SUB_A_L, SUB_A_memHL, SUB_A_A,
                     SBC_A_B, SBC_A_C, SBC_A_D, SBC_A_E, SBC_A_H, SBC_A_L, SBC_A_memHL, SBC_A_A,
                     AND_A_B, AND_A_C, AND_A_D, AND_A_E, AND_A_H, AND_A_L, AND_A_memHL, AND_A_A,
                     XOR_A_B, XOR_A_C, XOR_A_D, XOR_A_E, XOR_A_H, XOR_A_L, XOR_A_memHL, XOR_A_A,
                     OR_A_B, OR_A_C, OR_A_D, OR_A_E, OR_A_H, OR_A_L, OR_A_memHL, OR_A_A,
                     CP_A_B, CP_A_C, CP_A_D, CP_A_E, CP_A_H, CP_A_L, CP_A_memHL, CP_A_A,
                     RET_NZ,  POP_BC, JP_NZ_nn, JP_nn, CALL_NZ_nn, PUSH_BC, ADD_A_Im8, RST_00,
                     RET_Z,   RET,    JP_Z_nn, NULL,   CALL_Z_nn, CALL_nn,  ADC_A_Im8,  RST_08,
                     RET_NC,  POP_DE, JP_NC_nn, NULL,  CALL_NC_nn, PUSH_DE, SUB_A_Im8,  RST_10,
                     RET_C,   RETI,   JP_C_nn, NULL,   CALL_C_nn,  NULL,    SBC_A_Im8,  RST_18,
                     LDH_n_A, POP_HL, LDH_C_A, NULL, NULL,         PUSH_HL, AND_A_Im8,    RST_20,
                     ADD_SP_IM8, JP_memHL, LD_memnn_A, NULL, NULL,         NULL,    XOR_A_Im8,    RST_28,
                     LDH_A_n, POP_AF, LDH_A_C,    DI,   NULL,         PUSH_AF, OR_A_Im8,     RST_30,
                     LD_HL_SP_n, LD_SP_HL, LD_A_IM, EI, NULL, NULL,          CP_A_Im8,     RST_38};   
                      


instruction ext_ins[] = {
    
    RLC_B, RLC_C, RLC_D, RLC_E, RLC_H, RLC_L, RLC_memHL, RLC_A, 
    RRC_B, RRC_C, RRC_D, RRC_E, RRC_H, RRC_L, RRC_memHL, RRC_A,
    RL_B , RL_C , RL_D , RL_E , RL_H , RL_L , RL_memHL , RL_A , 
    RR_B , RR_C , RR_D , RR_E , RR_H , RR_L , RR_memHL , RR_A ,
    SLA_B, SLA_C, SLA_D, SLA_E, SLA_H, SLA_L, SLA_memHL, SLA_A, 
    SRA_B, SRA_C, SRA_D, SRA_E, SRA_H, SRA_L, SRA_memHL, SRA_A,
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
   


     
  



