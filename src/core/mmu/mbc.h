#ifndef MBC_H
#define MBC_H

#include <stdint.h>

uint8_t RAM_banks[16][0x2000];  // max 16 * 8KB ram banks (128KB)
uint8_t ROM_banks[512][0x4000];// max 512 * 16KB rom banks (8MB)

typedef enum {SRAM = 0x1, BATTERY = 0x2, RTC = 0x4, RUMBLE = 0x8} features;

/*  Setup a memory bank controller for the given
 *  cartridge type id. Returns 1 if successful,
 *  0 if not implemented or invalid. */
int setup_MBC(int no, unsigned ram_banks, char const *file_name);


void write_SRAM();
void read_SRAM();

/*  Placeholders for write/read function ptrs
 *  depending on MBC mode */
typedef uint8_t (*read_MBC_ptr)(uint16_t addr);
typedef void   (*write_MBC_ptr)(uint16_t addr, uint8_t val);

read_MBC_ptr read_MBC;
write_MBC_ptr write_MBC; 


#endif //MBC_H
