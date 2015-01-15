
#include <stdint.h>
#include "../../non_core/serial_io_transfer.h"


/* Setup TCP Client, and attempt to connect
 * to the server */
int setup_client(unsigned port) {
    
    return 0;
}

/*  Setup TCP Server, and wait for a single
 *  client to connect */
int setup_server(unsigned port) { 
 
    return 0;
}

/*  Send and Recieved byte */
int transfer(uint8_t data, uint8_t *recv, int ext) {
    
    return 0;
}


void quit_io() {
}

// Transfer when current GB is using external clock
// returns 1 if there is data to be recieved, 0 otherwise
int transfer_ext(uint8_t data, uint8_t *recv) {
    return 0;
}

// Transfer when current GB is using internal clock
// returns 0xFF if no external GB found
uint8_t transfer_int(uint8_t data) {
    return 0xFF;
}

