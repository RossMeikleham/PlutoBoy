#include "timers.h"
#include "interrupts.h"
#include "bits.h"
#include "mmu/mbc.h"

//Possible timer increment timer_frequencies in hz
#define TIMER_FREQUENCIES_LEN sizeof (timer_frequencies) / sizeof (long)
static const long timer_frequencies[] = {1024, 16, 64, 256}; 
static const long timer_frequencies_bits[] = {10, 4, 6, 8};
static long timer_frequency = -1;
static long timer_frequency_bits = -1;
static uint64_t clocks = 0;

uint16_t timer_counter = 0;
uint8_t previous_timer_counter = 0;
uint8_t previous_DIV = 0;

/* Change the timer frequency to another of the possible
 * frequencies, resets the timer_counter 
 * If frequency number selected isn't valid then nothing happens*/
void set_timer_frequency(unsigned int n) {
    if (n < TIMER_FREQUENCIES_LEN) {
        timer_frequency = timer_frequencies[n];
        timer_frequency_bits = timer_frequencies_bits[n];
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

    clocks += cycles;
    // Inc MBC3 RTC seconds
    if (clocks >= 4 * 1024 * 1024) {
        inc_sec_mbc3();         
        clocks -= 4 * 1024 * 1024;
    }

	uint8_t timer_control = io_mem[TAC_REG];

    previous_timer_counter = timer_counter & 0xFF;
    timer_counter += cycles;

    // Lower 8-bit overflows, increment the upper 8-bits i.e. the DIV register
    previous_DIV = io_mem[DIV_REG];
    while (timer_counter > 0xFF) {
        increment_div();
        timer_counter -= 0x100;
    }
        
	//Clock enabled
	if ((timer_control & BIT_2) != 0) {
        set_timer_frequency(timer_control & 3);
        // Check how many times the bit for the current frequency was "hit" 
        uint16_t previous_full_timer = (previous_DIV << 8) | previous_timer_counter;
        long full_timer = (io_mem[DIV_REG] << 8) | timer_counter;
        full_timer = previous_full_timer > full_timer ? 0x1 << 16 | full_timer : full_timer;

        long timer_incs = (full_timer >> timer_frequency_bits) - (previous_full_timer >> timer_frequency_bits); 

        for (long i = 0; i < timer_incs; i++)
        {
            increment_tima();
        }
	}
}
