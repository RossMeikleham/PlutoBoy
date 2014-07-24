#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

#define GB_CLOCK_SPEED_HZ  4194304 /* GameBoy Clock speed in HZ */
#define SGB_CLOCK_SPEED_HZ 4295454 /* Super GameBoy Clock speed in HZ */
#define CGB_CLOCK_SPEED_HZ 8388000 /* GameBoy Color Clock speed in HZ */


/*  Set and get the clockspeed in hz */
void set_clock_speed(long hz);
long get_clock_speed();

/* Update internal timers given the cycles executed since
 * the last time this function was called. */
void update_timers(long cycles);

#endif //TIMERS_H
