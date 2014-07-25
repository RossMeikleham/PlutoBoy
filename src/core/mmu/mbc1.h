#ifndef MBC1_H
#define MBC1_H


void setup_MBC1(int flags);

uint8_t read_MBC1(uint16_t addr);
void   write_MBC1(uint16_t addr, uint8_t val);


#endif
