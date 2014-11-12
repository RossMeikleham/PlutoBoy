// Handles Serial IO Transfers
// i.e. transfers through the GB link cable port

#include <stdint.h>
#include "interrupts.h"
#include "timers.h"
#include "serial_io.h"
#include "../non_core/serial_io_transfer.h"

static int transfer_in_progress = 0;
static int internal_clock = 0;
static unsigned cur_cycles = 0;
static unsigned gb_io_freq = 8192;

static uint8_t *recieved_location;
static uint8_t data_to_send;
static uint8_t *control;

int setup_serial_io(ClientOrServer cs, unsigned port) {
    if (cs == CLIENT) {
        return setup_client(port);
    } else if (cs == SERVER) {
        return setup_server(port);
    } else return 0;
}

/* Start a serial transfer, given then control signals
 * and byte of data to send, as well as a pointer of
 * where to store a recieved byte */
void start_transfer(uint8_t *control, uint8_t *data) {
    
    data_to_send = *data;
    recieved_location = data;
    transfer_in_progress = !!(*control & 0x80);
    internal_clock = *control & 0x1;
    if (internal_clock) {printf("yay\n");}
}

/* Add cycles to the serial transfer,
 * used to ensure when using internal clock,
 * data is transfered at the correct clock speed */
void inc_serial_cycles(unsigned cycles) {
    if (transfer_in_progress && internal_clock) {
        cur_cycles += cycles;
        if (cur_cycles >=  GB_CLOCK_SPEED_HZ / gb_io_freq) {
           cur_cycles = 0;
           printf("transferring\n");
           *recieved_location = transfer_int(data_to_send);
           raise_interrupt(IO_INT); 
           *control &= (0x7F);     
           transfer_in_progress = 0;
        }     
    }

    // Poll external transfer
    if (transfer_in_progress && !internal_clock) {
        uint8_t *result = NULL;
        int complete;
        if ((complete = transfer_ext(data_to_send, result))) {
            *recieved_location = *result;
            raise_interrupt(IO_INT);
            *control &= (0x7F);
            transfer_in_progress = 0;
        } 
    }

}
