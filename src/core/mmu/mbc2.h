#ifndef MBC2_H
#define MBC2_H


void setup_MBC2(int flags);

uint8_t read_MBC2(uint16_t addr);
void   write_MBC2(uint16_t addr, uint8_t val);


#endif
