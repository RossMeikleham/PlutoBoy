#ifndef MBC1_H
#define MBC1_H

typedef enum {ROM_ONLY, RAM, SRAM} MBC1_MODE;

void setup_MBC1(MBC1_MODE mode);

uint8_t read_MBC1(uint16_t addr);
void   write_MBC1(uint16_t addr, uint8_t val);


#endif
