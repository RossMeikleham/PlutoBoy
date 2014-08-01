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

/* Update internal timers given the cycles executed since
* the last time this function was called. */
void update_timers(long cycles);

#endif //TIMERS_H
