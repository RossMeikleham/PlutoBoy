#include "memory.h"
#include <stdint.h>


/*  -------------IO ports/registers ------------------*/
#define P1_REG 0xFF00 /*  Register for reading joy pad info */
#define SB_REG 0xFF01 /*  Serial transfer data */
#define SC_REG 0xFF02 /*  SIO control */
#define DIV_REG 0xFF04 /*  Divider register*/
#define TIMA_REG 0xFF05 /*  Timer Counter */
#define TMA_REG 0xFF06 /*  Timer Modulo  */
#define TAC_REG 0xFF07 /*  Timer Control */
#define IF_FLAG 0xFF0F /*  Interrupt Flag */

/*   Sound Mode 1 registers */
#define NR_10_REG 0xFF10 /*  Sweep */
#define NR_11_REG 0xFF11 /*  Length wave pattern*/
#define NR_12_REG 0xFF12 /*  Envelope */
#define NR_13_REG 0xFF12 /*  Frequency Lo */
#define NR_14_REG 0xFF14 /*  Frequency Hi */

/*   Sound Mode 2 registers */
#define NR_21_REG 0xFF16 /*  Length wave pattern*/
#define NR_22_REG 0xFF17 /*  Envelope */
#define NR_23_REG 0xFF18 /*  Frequency Lo */
#define NR_24_REG 0xFF19 /*  Frequency Hi */

/*   Sound Mode 3 registers */
#define NR_30_REG 0xFF1A /*  Sound on/off*/
#define NR_31_REG 0xFF1B /*  Sound length */
#define NR_32_REG 0xFF1C /*  Select output level */
#define NR_33_REG 0xFF1D /*  Frequency Lo */
#define NR_34_REG 0xFF1E /*  Frequency Hi */

/*   Sound Mode 4 registers */
#define NR_41_REG 0xFF20 /*  Sound length */
#define NR_42_REG 0xFF21 /*  Envelope */
#define NR_43_REG 0xFF22 /*  Polynomial Counter */
#define NR_44_REG 0xFF23 /*  Counter/Consecutive; initial */

#define NR_50_REG 0xFF24 /*  Channel Control/on-off/Volume */
#define NR_51_REG 0xFF25 /*  Selection of Sound output terminal */
#define NR_52_REG 0xFF26 /*  Sound on/off */

/*  Waveform Storage for Arbitrary Sound data */
#define WAVE_PATTERN_RAM_START 0xFF30
#define WAVE_PATTERN_RAM_END   0xFF3F

/* ------Screen/Graphics register locations------- */
#define LCDC_REG 0xFF40 /*  LCD Control */
#define STAT_REG 0xFF41 /*  LCD status */
#define SCROLL_Y_REG 0xFF42 /*  8 bit value to scroll BG Y pos */
#define SCROLL_X_REG 0xFF43 /*  8 bit value to scroll BG X pos */
#define LY_REG 0xFF44 /*  LCDC Y-Coordinate */
#define LYC_REG 0xFF45 /*  LY Compare */
#define DMA_REG 0xFF46 /*  DMA Transfer and Start Address */
#define BGP_REF 0xFF47 /*  BG and Window Palette Data */
#define OBP0_REG 0xFF48 /*  Object Palette 0 Data */
#define OBP1_REG 0xFF49 /*  Object Palette 1 Data */
#define WY_REG 0xFF4A /*  Window Y Position; 0 <= WY <= 143*/
#define WX_REG 0xFF4B /*  Window X Position; 0 <= WX <= 166 */

#define IE_REG 0xFFFF /*  Interrupt Enable Register */

/* ************************************************** */



typedef enum {RIGHT = 0x1, LEFT = 0x2, UP = 0x4, DOWN = 0x8, 
    A = 0x10, B = 0x20, SELECT = 0x40, START = 0x80} button_t;

typedef struct {
    union {
        { int start:1; int select:1; int a:1; int b:1;
          int up:1; int down:1; int left:1; int right:1;
        }
        { int all;
        }
    } 
} joypad_t;

joypad_t joypad;
uint8_t joypad_reg_value = 0x0;

void joypad_pressed(button_t button) {
    uint8_t joypad_reg_value = 0x0;

}


/*  Reset current keys pressed */
void joypad_reset() {
    joypad.all =0x0;
}


/*  Write the current joypad state
 *  to the joypad register in memory */
void joypad_write_to_mem() {
}




