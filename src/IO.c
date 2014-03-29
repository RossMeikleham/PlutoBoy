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


#define BIT_0 0x1
#define BIT_1 0x2
#define BIT_2 0x4
#define BIT_3 0x8
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80


typedef enum {RIGHT = 0x1, LEFT = 0x2, UP = 0x4, DOWN = 0x8, 
    A = 0x10, B = 0x20, SELECT = 0x40, START = 0x80} button_t;


int joypad;
uint8_t joypad_reg_value = 0x0;

void joypad_pressed(button_t button) {
    uint8_t joypad &= button;

}


/*  Reset current keys pressed */
void joypad_reset() {
    joypad  =0x0;
}


/*  Write the current joypad state
 *  to the joypad register in memory */
void joypad_write_to_mem() {
    setMem(P1_REG, joypad);
}




/*  Managing FF40 LCDC control reg */
void apply_lcd_control() {
    uint8_t lcd_ctrl = getMem(LCDC_REG);
    
    int operation = lcd_ctrl & BIT_7; /* 0 no display, 1 otherwise */
    int win_tile_map = lcd_ctrl & BIT_6; /* 0: 0x9800-0x9BFF, 1: 0x9C00-0x9FFF */
    int win_display = lcd_ctrl & BIT_5; /* 0: off, 1: on */
    int bg_win_tile = lcd_ctrl & BIT_4; /* 0: 0x8800-0x97FF, 1: 0x8000-0x8FFF */
    int bg_tile_map = lcd_ctrl & BIT_3; /* 0: 0x9800-0x97FF, 1: 0x9C00-0x9FFF */
    int sprite_size = lcd_ctrl & BIT_2: /* 0: 8x8. 1: 8*x16  (width * height) */
    int sprite_display = lcd_ctrl & BIT_1; /* 0: off, 1: on */
    int bg_window_display = ldc_ctrl & BIT_0; /* 0 off, 1: on  */
}

void lcd_status() {
    uint8_t lcd_stat = getMem(STAT_REG);

    int lyc_equals_ly_coincidence = lcd_stat & BIT_6;
    int oam_search = lcd_stat * BIT_5;
    int v_blank = lcd_stat & BIT_4;
    int h_blank = lcd_stat & BIT_3;
    int coincidence_flag = lcd_stat & BIT_2; /* 0: LYC != LCDC LY, 1: equals */
    int mode_flag = lcd_stat & (BIT_1 | BIT_0);
    /*  0 : During H_Blank. Entire Display RAM (0x8000-0x9FFF) can be accessed */
    /*  1 : During V_BLANK. Entire Display RAM (0x8000-0x9FFF) can be accessed 
     *  2 : During Searching OAM_RAM (0xFE00-0xFE9F), OAM cannot be accessed */
    /*  3 : During Transferring Data to LCD Driver. CPU cannot access OAM
     *      and display RAM during this period */
}



/*  Get amount of scrolling in X/Y directions */
uint8_t get_scroll_y() {
    return getMem(SCY_REG);
}

uint8_t get_scroll_x() {
    return getMem(SCX_REG);
}


/*  Vertical line which present data transferred
 *  to the LCD Driver- LY can take any value 0-153, 
 *  144-153 indicate V_Blank period. Writing will
 *  resset the counter */
uint8_t get_ly() {
    return getMem(LY_REG);
}



/*  Compares with LY, if values are the same, causes STAT
 *  to set the coincident flag */
uint8_t get_lyc() {
    return getMem(LYC_REG);
}


/*  DMA Transfer  */
/*  (40*28 bit) from internal ROM or RAM (0x0000-0xF19F)
 *  to the OAM (0xFE00-0xFE9F) */
void dma() {
    
}


uint8_t bg_palette() {
    /*  7-6 Data for Darkest colour */
    /*  5-4 Data for Dark colour */
    /*  3-2 Data for Light Colour */
    /*  1-0 Data for Lightest Colour */
}


uint8_t OBP0() {
    /*  Selects colours for sprite palette 0. Works exactly as BGP
     *  (0xFF47) except 0 is transparent */
}

uint8_t OBP1() {
   /* Selects the colours for sprite palette 1. Works exactly as OBPO
    * (0xFF48) */
}

uint8_t get_win_y_pos() {
    /*  0<=WY<=143 */
    return getMem(WY_REG);
}

uint8_t get_win_x_pos() {
    /*  0<=WX<=166 */
    return getMem(WX_REG);
}


void interrupt_enable() {
    uint8_t interrupt = getMem(IE_REG);

    int pin_high_to_low = interrupt & BIT_4;
    int serial_IO_transfer_complete = interrupt & BIT_3;
    int timer_overflow = interrupt& BIT_2;
    int lcdc = interrupt & BIT_1;
    int v_blank = interrupt & BIT_0;
}

























