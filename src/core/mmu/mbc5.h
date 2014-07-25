#ifndef MBC5_H
#define MBC5_H


void setup_MBC5(int flags);
uint8_t read_MBC5(uint16_t addr);
void   write_MBC5(uint16_t addr, uint8_t val);

#endif //MBC5_H
