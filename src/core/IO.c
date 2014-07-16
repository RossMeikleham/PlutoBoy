#include "cpu.h"
#include "graphics.h"
#include "IO.h"
#include "memory.h"
#include <stdint.h>
#include "memory_layout.h"
#include <stdio.h>
typedef struct {
    uint8_t isr_addr; /* Interrupt Service Routine Address */
    uint8_t flag; /* bit set to compare with IF_FLAG to 
                    check if interrupt has been raised */
} Interrupt;


static uint8_t io_mem[256]= {
		0xCF, 0x00, 0x7E, 0xFF, 0xD3, 0x00, 0x00, 0xF8,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
		0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00,
		0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
		0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x71, 0x72, 0xD5, 0x91, 0x58, 0xBB, 0x2A, 0xFA,
		0xCF, 0x3C, 0x54, 0x75, 0x48, 0xCF, 0x8F, 0xD9,
		0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFC,
		0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x2B, 0x0B, 0x64, 0x2F, 0xAF, 0x15, 0x60, 0x6D,
		0x61, 0x4E, 0xAC, 0x45, 0x0F, 0xDA, 0x92, 0xF3,
		0x83, 0x38, 0xE4, 0x4E, 0xA7, 0x6C, 0x38, 0x58,
		0xBE, 0xEA, 0xE5, 0x81, 0xB4, 0xCB, 0xBF, 0x7B,
		0x59, 0xAD, 0x50, 0x13, 0x5E, 0xF6, 0xB3, 0xC1,
		0xDC, 0xDF, 0x9E, 0x68, 0xD7, 0x59, 0x26, 0xF3,
		0x62, 0x54, 0xF8, 0x36, 0xB7, 0x78, 0x6A, 0x22,
		0xA7, 0xDD, 0x88, 0x15, 0xCA, 0x96, 0x39, 0xD3,
		0xE6, 0x55, 0x6E, 0xEA, 0x90, 0x76, 0xB8, 0xFF,
		0x50, 0xCD, 0xB5, 0x1B, 0x1F, 0xA5, 0x4D, 0x2E,
		0xB4, 0x09, 0x47, 0x8A, 0xC4, 0x5A, 0x8C, 0x4E,
		0xE7, 0x29, 0x50, 0x88, 0xA8, 0x66, 0x85, 0x4B,
		0xAA, 0x38, 0xE7, 0x6B, 0x45, 0x3E, 0x30, 0x37,
		0xBA, 0xC5, 0x31, 0xF2, 0x71, 0xB4, 0xCF, 0x29,
		0xBC, 0x7F, 0x7E, 0xD0, 0xC7, 0xC3, 0xBD, 0xCF,
		0x59, 0xEA, 0x39, 0x01, 0x2E, 0x00, 0x69, 0x00
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

/*  Check if any interrupts need to be handled, and handle them 
 *  returns 1 if a PC freeze needs to take place, 0 otherwise*/
int check_interrupts() {

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

                    unhalt_cpu();
                    return 0;
                }
                //If Gameboy, SGB or Gameboy pocket PC is halted
                // and interrupts disabled, cpu is unhalted and
                // bug in the original system causes the next
                // 1st byte of instruction to be repeated
                else if (is_halted()) {
                    unhalt_cpu();
                    return 1;
                }
            }
        }
    }

    return 0;
}

// Keypad is written to, update register with state
// Not implemented yet, so keys set to 1 (off) for now
void update_joypad(uint8_t val) {
    if (val == 0x20) {
        io_mem[GLOBAL_TO_IO_ADDR(P1_REG)] = ~0x10;
    } else if (val == 0x10) {
        io_mem[GLOBAL_TO_IO_ADDR(P1_REG)] = ~0x20;
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
        case P1_REG  : update_joypad(val); break;
        //case SC_REG : if (val == 0x81) {printf("%c",io_mem[GLOBAL_TO_IO_ADDR(SB_REG)]);} break;
        case TIMA_REG : break;
        case TMA_REG  : break;
        case TAC_REG  : break;
        /*  Attempting to set DIV reg resets it to 0 */
        case DIV_REG  : io_mem[io_addr] = 0; break;
        /*  Attempting to set LY reg resets it to 0  */
        case LY_REG   : io_mem[io_addr] = 0; break;
        /*  Perform direct memory transfer  */
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
    uint8_t ly = (io_mem[IO_LY_REG] + 1) % 154;    
    io_mem[IO_LY_REG] = ly;

    //io_mem[IO_LY_REG] = ly;
    if (ly < 144) {
        draw_row();
    } else if (ly == 144) { /*  V-Blank interrupt */
       io_mem[GLOBAL_TO_IO_ADDR(IF_FLAG)] |= BIT_0;
    }

    //io_mem[IO_LY_REG] = (ly + 1) % 154;
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
