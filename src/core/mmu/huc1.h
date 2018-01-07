#ifndef HUC1_H
#define HUC1_H


void setup_HUC1(int flags);
uint8_t read_HUC1(uint16_t addr);
void   write_HUC1(uint16_t addr, uint8_t val);

#endif //HUC1_H
