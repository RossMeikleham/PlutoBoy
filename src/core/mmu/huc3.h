#ifndef HUC3_H
#define HUC3_H


void setup_HUC3(int flags);
uint8_t read_HUC3(uint16_t addr);
void   write_HUC3(uint16_t addr, uint8_t val);

#endif //HUC3_H
