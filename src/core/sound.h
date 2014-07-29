#include <stdint.h>

#ifdef __cplusplus 
extern "C" {
#endif

void sound_add_cycles(unsigned cycles);

void init_apu();

void write_apu(uint16_t addr, uint8_t val);

uint8_t read_apu(uint16_t addr);

void end_frame();


#ifdef __cplusplus
}
#endif
