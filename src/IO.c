#include "memory.h"
#include "graphics.h"
#include "IO.h"
#include <stdint.h>




int check_interrupt() {
    uint8_t interrupt;
    /*  Check interrupt has been raised */
    if ((interrupt = get_mem(IF_FLAG) & 0x0F)) {
        /*  Check VBlank first */
        if (interrupt & BIT_0);
        /*  Check LCDC  */
        if (interrupt & BIT_1);
        /*  Check Timer Overflow*/
        if (interrupt & BIT_2) {
            set_mem(TIMA_REG, get_mem(TMA_REG ));
        }
        /*  Check Serial I/O transfer complete */
        if (interrupt & BIT_3);
        /*  Check transion from high low pins */
        if (interrupt & BIT_4);

    }

    return !!interrupt;
}

/*  Increments the TIMA register
 *  returns 1 if overflow causing timer interrupt */
int increment_tima() {
    uint8_t tima = get_mem(TIMA_REG);
    set_mem(TIMA_REG, ++tima);
    return tima == 0;
}

/*  Returns 0 if not in V-Blank period, 
 *  1 if in V-Blank period */
int increment_ly() {
    uint8_t ly = (get_mem(LY_REG)+1) % 154;
    set_mem(LY_REG, ly);
    if (ly < 144) {
        draw_row(ly);
    }
    return ly >= 144;
}

typedef enum {RIGHT = 0x1, LEFT = 0x2, UP = 0x4, DOWN = 0x8, 
    A = 0x10, B = 0x20, SELECT = 0x40, START = 0x80} button_t;


int joypad;
uint8_t joypad_reg_value = 0x0;

void joypad_pressed(button_t button) {
    

}


/*  Reset current keys pressed */
void joypad_reset() {
    joypad  =0x0;
}


/*  Write the current joypad state
 *  to the joypad register in memory */
void joypad_write_to_mem() {
    set_mem(P1_REG, joypad);
}



/*  Managing FF40 LCDC control reg */
lcd_ctrl_t get_lcd_control() {
    uint8_t lcd_ctrl_val = get_mem(LCDC_REG);
    lcd_ctrl_t lcd_ctrl;
    
    lcd_ctrl.operation = lcd_ctrl_val & BIT_7; /* 0 no display, 1 otherwise */
    lcd_ctrl.win_tile_map = lcd_ctrl_val & BIT_6; /* 0: 0x9800-0x9BFF, 1: 0x9C00-0x9FFF */
    lcd_ctrl.win_display = lcd_ctrl_val & BIT_5; /* 0: off, 1: on */
    lcd_ctrl.bg_win_tile = lcd_ctrl_val & BIT_4; /* 0: 0x8800-0x97FF, 1: 0x8000-0x8FFF */
    lcd_ctrl.bg_tile_map = lcd_ctrl_val & BIT_3; /* 0: 0x9800-0x97FF, 1: 0x9C00-0x9FFF */
    lcd_ctrl.sprite_size = lcd_ctrl_val & BIT_2; /* 0: 8x8. 1: 8*x16  (width * height) */
    lcd_ctrl.sprite_display = lcd_ctrl_val & BIT_1; /* 0: off, 1: on */
    lcd_ctrl.bg_window_display = lcd_ctrl_val & BIT_0; /* 0 off, 1: on  */

    return lcd_ctrl;
}

void lcd_status() {
    uint8_t lcd_stat = get_mem(STAT_REG);

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
    return get_mem(SCROLL_Y_REG);
}

uint8_t get_scroll_x() {
    return get_mem(SCROLL_X_REG);
}


/*  Vertical line which present data transferred
 *  to the LCD Driver- LY can take any value 0-153, 
 *  144-153 indicate V_Blank period. Writing will
 *  resset the counter */
uint8_t get_ly() {
    return get_mem(LY_REG);
}



/*  Compares with LY, if values are the same, causes STAT
 *  to set the coincident flag */
uint8_t get_lyc() {
    return get_mem(LYC_REG);
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
    return 0;
}


uint8_t OBP0() {
    /*  Selects colours for sprite palette 0. Works exactly as BGP
     *  (0xFF47) except 0 is transparent */
    return 0;
}

uint8_t OBP1() {
   /* Selects the colours for sprite palette 1. Works exactly as OBPO
    * (0xFF48) */
   return 0;
}

uint8_t get_win_y_pos() {
    /*  0<=WY<=143 */
    return get_mem(WY_REG);
}

uint8_t get_win_x_pos() {
    /*  0<=WX<=166 */
    return get_mem(WX_REG);
}


void interrupt_enable() {
    uint8_t interrupt = get_mem(IE_REG);

 }

























