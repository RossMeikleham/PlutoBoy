
#include <stdint.h>

int setup_client(unsigned port);
int setup_server(unsigned port);

uint8_t transfer_int(uint8_t data);
int transfer_ext(uint8_t data, uint8_t *recv);
