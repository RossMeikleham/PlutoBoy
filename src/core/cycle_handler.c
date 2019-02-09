#include "cycle_handler.h"

#include "timers.h"

bool current_cycles_valid = true;
long cur_io_cycles = 0;
long io_cycles_til_change = 0;

static void min_cycles_til_next_external_state_change() {
    long timer_cycles = timer_cycles_til_external_state_change();

    long cycles = timer_cycles;
    io_cycles_til_change = cgb_speed ? cycles >> 1: cycles;
}

void invalidate_current_cycle_run() {
    update_timers(cur_io_cycles);
    cur_io_cycles = 0;

    current_cycles_valid = true;
    min_cycles_til_next_external_state_change();
}
