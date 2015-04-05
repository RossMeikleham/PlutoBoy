#ifndef HDMA_H
#define HDMA_H

int hdma_in_progress;
int gdma_in_progress;
int bytes_transferred; // no of bytes transferred in current dma


//Start either HDMA or GDMA transfer
void start_gbc_dma(uint16_t s_addr, uint16_t e_addr, unsigned b, int type);

long perform_hdma();

#endif //HDMA_H
