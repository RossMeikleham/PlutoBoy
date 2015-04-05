#ifndef HDMA_H
#define HDMA_H

int hdma_in_progress;
int gdma_in_progress;
int bytes_transferred; // no of bytes transferred in current dma
uint16_t hdma_source;
uint16_t hdma_dest;
uint16_t hdma_bytes;


void check_cgb_dma(uint8_t value);

long perform_hdma();

void perform_gdma(uint8_t value);

#endif //HDMA_H
