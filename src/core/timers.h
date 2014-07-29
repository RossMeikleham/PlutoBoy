#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include "bits.h"
#include "mmu/memory.h"
#include "memory_layout.h"
#include <stdio.h>

#define GB_CLOCK_SPEED_HZ  4194304 /* GameBoy Clock speed in HZ */
#define SGB_CLOCK_SPEED_HZ 4295454 /* Super GameBoy Clock speed in HZ */
#define CGB_CLOCK_SPEED_HZ 8388000 /* GameBoy Color Clock speed in HZ */
#define DIV_TIMER_INC_FREQUENCY 16382


long divider_counter;
long clock_speed;
long timer_frequency;
long timer_counter;

void setup_timers();

/*  Set and get the clockspeed in hz */
void set_clock_speed(long hz);
long get_clock_speed();

void increment_div();

void increment_tima();

/* Change the timer frequency to another of the possible
 * frequencies, resets the timer_counter 
 * If frequency number selected isn't valid then nothing happens*/
void set_timer_frequency(unsigned int n);

inline void update_divider_reg(long cycles) {

    divider_counter += cycles;
    // Increment div at a frequency of 16382hz
    long max_counter = clock_speed / DIV_TIMER_INC_FREQUENCY;
    while (divider_counter >= max_counter) {
        increment_div();
        divider_counter -= clock_speed / max_counter;
    }
}


/* Update internal timers given the cycles executed since
 * the last time this function was called. */
inline void update_timers(long cycles) { 
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
        while (timer_counter >= (clock_speed / timer_frequency)) {
            timer_counter -= (clock_speed / timer_frequency);
            increment_tima();
            set_timer_frequency(timer_control & 3);
        }
    }
}


#endif //TIMERS_H
