// Handles Serial IO Transfers
// i.e. transfers through the GB link cable port
#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include <stdint.h>

typedef enum {CLIENT = 0, SERVER = 1, NO_CONNECT = 2} ClientOrServer;


int setup_serial_io(ClientOrServer cs, unsigned port);

/* Start a serial transfer, given then control signals
 * and byte of data to send, as well as a pointer of
 * where to store a recieved byte */
void start_transfer(uint8_t *control, uint8_t *data);

/* Add cycles to the serial transfer,
 * used to ensure when using internal clock,
 * data is transfered at the correct clock speed */
void inc_serial_cycles(unsigned cycles);

#endif
