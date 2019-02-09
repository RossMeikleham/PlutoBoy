#ifndef CYCLE_HANDLER_H
#define CYCLE_HANDLER_H

#include <stdbool.h>

#include "timers.h"

extern bool current_cycles_valid;
extern long cur_io_cycles;;
extern long io_cycles_til_change;

// If IO memory is altered in that the cycles til
// next interrupt could be changed
void invalidate_current_cycle_run();

#endif
