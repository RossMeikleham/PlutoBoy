#include "timers.h"
#include "interrupts.h"
#include "bits.h"
#include "mmu/mbc.h"

//Possible timer increment timer_frequencies in hz
#define TIMER_FREQUENCIES_LEN sizeof (timer_frequencies) / sizeof (long)
static const long timer_frequencies[] = {1024, 16, 64, 256}; 

static long timer_frequency = -1;
static uint64_t clocks = 0;

uint8_t timer_counter = 0;
uint8_t previous_timer_counter = 0;
uint8_t previous_DIV = 0;

/* Change the timer frequency to another of the possible
 * frequencies, resets the timer_counter 
 * If frequency number selected isn't valid then nothing happens*/
void set_timer_frequency(unsigned int n) {
    if (n < TIMER_FREQUENCIES_LEN) {
        timer_frequency = timer_frequencies[n];
    }
}


long get_timer_frequency() {
    //return cgb_speed ? timer_frequency / 2 : timer_frequency;
    return timer_frequency;
}

/*  Increments the TIMA register
 *  if causes overflow, timer interrupt is raised*/
void increment_tima() {

    uint8_t tima = io_mem[TIMA_REG] + 1;

    if (tima == 0) { //Overflow
        tima = io_mem[TMA_REG];
        raise_interrupt(TIMER_INT);
    }
    io_mem[TIMA_REG] = tima;
    
}


/*  Increment DIV register 
 *  should be incremented at a frequency of 16382
 *  (once every 256 clock cycles)*/
void increment_div() {
    io_mem[DIV_REG] += 1;
}


/*  
void update_divider_reg(long cycles) {
    
	divider_counter += cycles;
	long div_cycles = cgb_speed ? 128 : 256;
	while (divider_counter >= div_cycles) {
		increment_div();
		divider_counter -= div_cycles;
	}
}*/


/* Update internal timers given the cycles executed since
* the last time this function was called. */
void update_timers(long cycles) {

    if (cgb_speed)
    {
        cycles *= 2;
    }

    clocks += cycles;
    // Inc MBC3 RTC seconds
    if (clocks >= 4 * 1024 * 1024) {
        inc_sec_mbc3();         
        clocks -= 4 * 1024 * 1024;
    }

	uint8_t timer_control = io_mem[TAC_REG];

    previous_timer_counter = timer_counter;
    timer_counter += cycles;
    
    // Lower 8-bit overflows, increment the upper 8-bits i.e. the DIV register
    previous_DIV = io_mem[DIV_REG];
    if (timer_counter < previous_timer_counter) {
        increment_div();
    }
        
    //printf("0x%x 0x%x, freq: 0x%x\n", previous_DIV, previous_timer_counter, timer_frequency);
    //printf("0x%x 0x%x\n\n", io_mem[DIV_REG], timer_counter);
    
    //uint16_t full_timer = (io_mem[DIV_REG] << 8) | timer_counter;
    uint16_t previous_full_timer = (previous_DIV << 8) | previous_timer_counter;

	//Clock enabled
	if ((timer_control & BIT_2) != 0) {
	    set_timer_frequency(timer_control & 3);
		/*if (timer_frequency == -1) { // If timer not set
			set_timer_frequency(timer_control & 3);
		}*/

        uint16_t full_timer = (io_mem[DIV_REG] << 8) | timer_counter;

        //printf("prev: 0x%x new 0x%x\n", previous_full_timer, full_timer);
        // Clock Frequency bit gone from high to low
        if (((previous_full_timer & (timer_frequency >> 1)) != 0) &&
            ((full_timer & (timer_frequency >> 1)) == 0)) {
            //printf("inc tima clock\n");
            increment_tima();
		}
	}
}
