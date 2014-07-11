#include "cpu.h"
#include "graphics.h"
#include "IO.h"
#include "memory.h"
#include <stdint.h>
#include "memory_layout.h"

typedef struct {
    uint8_t isr_addr; /* Interrupt Service Routine Address */
    uint8_t flag; /* bit set to compare with IF_FLAG to 
                    check if interrupt has been raised */
} Interrupt;


static uint8_t io_mem[256]= {
    0xCF, 0x00, 0x7E, 0xFF, 0xAD, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x80, 0xBF, 0xF0, 0xFF,
    0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF,
    0xBF, 0xFF, 0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xFE,
    0x0E, 0x7F, 0x00, 0xFF, 0x58, 0xDF, 0x00, 0xEC, 0x00, 0xBF,
    0x0c, 0xED, 0x03, 0xF7, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xC1, 0x20, 0x00, 0x00,
    0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const Interrupt interrupts[] = { 
    {.flag = BIT_0, .isr_addr = VBLANK_ISR_ADDR},
    {.flag = BIT_1, .isr_addr = LCDC_ISR_ADDR},
    {.flag = BIT_2, .isr_addr = TIMER_ISR_ADDR},
    {.flag = BIT_3, .isr_addr = IO_ISR_ADDR},
    {.flag = BIT_3, .isr_addr = HIGH_LOW_ISR_ADDR}
   };

#define INTERRUPTS_LEN sizeof (interrupts) / sizeof (Interrupt)


void set_lcd_interrupt() {
    io_mem[GLOBAL_TO_IO_ADDR(IF_FLAG)] |= BIT_1;    
}

/*  Check if any interrupts need to be handled, and handle them */
void check_interrupts() {

    #define IO_IF_FLAG GLOBAL_TO_IO_ADDR(IF_FLAG)
    #define IO_IE_REG  GLOBAL_TO_IO_ADDR(IE_REG)
    // Get all interrupts which flag has been set and are enabled
    uint8_t if_flag = io_mem[IO_IF_FLAG];
    uint8_t possible_interrupts = if_flag & io_mem[IO_IE_REG] & 0xF;
 
    if (possible_interrupts != 0) {
        /* Go through each interrupt and check if it has been raised */
        for (unsigned long i = 0; i < INTERRUPTS_LEN; i++) {

            uint8_t flag = interrupts[i].flag;
            if ((flag & possible_interrupts) != 0) {
                /*  Still need to check if master override is not in place */
                if (master_interrupts_enabled()) {
                    /* Unset interrupt flag for interrupt being serviced
                     * unset master interrupts so interrupt handler routine
                     * isn't unecessarily interrupted and then call
                     * the interrupt handler */
                    io_mem[IO_IF_FLAG] = if_flag & ~flag;
                    master_interrupts_disable(); 
                    restart(interrupts[i].isr_addr);
                }
                /* If CPU is halted, even if MIE if off, 
                 * interrupt unhalts the cpu*/
                unhalt_cpu();
            }
        }
    }
}


/* Transfer 160 bytes to sprite memory starting from
 * address XX00 */
static inline void dma_transfer(uint8_t val) {
    
    uint16_t source_addr = val << 8;
    for (int i = 0; i < 0xA0; i++) {
        set_mem(SPRITE_ATTRIBUTE_TABLE_START + i, get_mem(source_addr + i));    
    }
}

void io_set_mem(uint8_t io_addr, uint8_t val) {

    uint16_t global_addr = IO_TO_GLOBAL_ADDR(io_addr);
    io_mem[io_addr] = val;
    switch (global_addr) {
        /*  Timers */
        case TIMA_REG : break;
        case TMA_REG  : break;
        case TAC_REG  : break;
        /*  Attempting to set DIV reg resets it to 0 */
        case DIV_REG  : io_mem[io_addr] = 0; break;
        /*  Attempting to set LY reg resets it to 0  */
        //case LY_REG   : io_mem[io_addr] = 0; break;
        /*  Performa direct memory transfe  */
        case DMA_REG  : dma_transfer(val); break;
    }
}

uint8_t io_get_mem(uint8_t io_addr) {
    return io_mem[io_addr];
}

/*  Increments the TIMA register
 *  if causes overflow, timer interrupt is raised*/
void increment_tima() {

    #define IO_TIMA_REG GLOBAL_TO_IO_ADDR(TIMA_REG)
    io_mem[IO_TIMA_REG] += 1;
    /*  Overflow */
    if (io_mem[IO_TIMA_REG] == 0) {
        io_mem[IO_TIMA_REG] = io_mem[GLOBAL_TO_IO_ADDR(TMA_REG)];
        io_mem[GLOBAL_TO_IO_ADDR(IF_FLAG)] |= BIT_2;
    } 
}

/*  Increcements LY registers
 *  if LY becomes greater than 143 then
 *  VBlank interrupt is raised */
void increment_ly() {

    #define IO_LY_REG GLOBAL_TO_IO_ADDR(LY_REG)
    uint8_t ly = (io_mem[IO_LY_REG] +1) % 154;
    io_mem[IO_LY_REG] = ly;

    if (ly < 144) {
        draw_row();
    } else { /*  V-Blank interrupt */
       io_mem[GLOBAL_TO_IO_ADDR(IF_FLAG)] |= BIT_0;
    }
}

/*  Increment DIV register 
 *  should be incremented at a frequency of 16382
 *  (once every 256 clock cycles)*/
void increment_div() {
    io_mem[GLOBAL_TO_IO_ADDR(DIV_REG)] += 1;
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
    //set_mem(P1_REG, joypad);
}

/*  Get amount of scrolling in X/Y directions */
uint8_t get_scroll_y() {
    return io_mem[GLOBAL_TO_IO_ADDR(SCROLL_Y_REG)];
}

uint8_t get_scroll_x() {
    return io_mem[GLOBAL_TO_IO_ADDR(SCROLL_X_REG)];
}


uint8_t get_win_y_pos() {
    /*  0<=WY<=143 */
    return io_mem[GLOBAL_TO_IO_ADDR(WY_REG)];
}

uint8_t get_win_x_pos() {
    /*  0<=WX<=166 */
    return io_mem[GLOBAL_TO_IO_ADDR(WX_REG)];
}
