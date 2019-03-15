#ifndef HDMA_H
#define HDMA_H

extern int hdma_in_progress;
extern int gdma_in_progress;
extern int bytes_transferred; // no of bytes transferred in current dma
extern uint16_t hdma_source;
extern uint16_t hdma_dest;
extern uint16_t hdma_bytes;


void check_cgb_dma(uint8_t value);

long perform_hdma();

void perform_gdma(uint8_t value);

#endif //HDMA_H
