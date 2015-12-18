#ifndef MBC3_H
#define MBC3_H

void set_rtc_MBC3();
void write_rtc_MBC3();

void inc_rtc_second();

void setup_MBC3(int flags);
uint8_t read_MBC3(uint16_t addr);
void   write_MBC3(uint16_t addr, uint8_t val);

#endif //MBC3_H
