#include "timers.h"
#include "interrupts.h"
#include "bits.h"


//Possible timer increment timer_frequencies in hz
#define TIMER_FREQUENCIES_LEN sizeof (timer_frequencies) / sizeof (long)
static const long timer_frequencies[] = {1024, 16, 64, 256}; 

static long timer_frequency = -1;
static long timer_counter = 0;


/* Change the timer frequency to another of the possible
 * frequencies, resets the timer_counter 
 * If frequency number selected isn't valid then nothing happens*/
void set_timer_frequency(unsigned int n) {
    if (n < TIMER_FREQUENCIES_LEN) {
        timer_frequency = timer_frequencies[n];
    }
}

long get_timer_frequency() {
    return cgb_speed ? timer_frequency / 2 : timer_frequency;
}

/*  Increments the TIMA register
 *  if causes overflow, timer interrupt is raised*/
void increment_tima() {

    uint8_t tima = get_mem(TIMA_REG) + 1;

    if (tima == 0) { //Overflow
        tima = get_mem(TMA_REG);
        raise_interrupt(TIMER_INT);
    }
    io_write_override(GLOBAL_TO_IO_ADDR(TIMA_REG), tima);
    
}


/*  Increment DIV register 
 *  should be incremented at a frequency of 16382
 *  (once every 256 clock cycles)*/
void increment_div() {
    io_write_override(GLOBAL_TO_IO_ADDR(DIV_REG), get_mem(DIV_REG) + 1);
}


void update_divider_reg(long cycles) {
    
    static long divider_counter = 0;

	divider_counter += cycles;
	long div_cycles = cgb_speed ? 128 : 256;
	while (divider_counter >= div_cycles) {
		increment_div();
		divider_counter -= div_cycles;
	}
}


/* Update internal timers given the cycles executed since
* the last time this function was called. */
void update_timers(long cycles) {
	uint8_t timer_control = get_mem(TAC_REG);

	update_divider_reg(cycles);
	//Clock enabled
	if ((timer_control & BIT_2) != 0) {
		if (timer_frequency == -1) { // If timer not set
			set_timer_frequency(timer_control & 3);
		}
		timer_counter += cycles;
		/* Once timer incremented, check for changes in timer frequency,
		* and reset timer */
		while (timer_counter >= get_timer_frequency()) {
			timer_counter -= get_timer_frequency();
			increment_tima();
			set_timer_frequency(timer_control & 3);
		}
	}
}

