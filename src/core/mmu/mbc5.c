#include "mbc.h"
#include "mbc5.h"
#include "memory.h"
#include <stdint.h>
#include <stdio.h>
/* Memory Bank Controller 5 
 * 8MB ROM 128KB RAM
 *
*/

static int cur_RAM_bank = 0; // Current ROM bank 0x0 - 0x1F
static int rom_bank_hi_bit = 0; //Store high (bit 8) for ROM bank
static uint8_t rom_bank_low = 1; // Store lower 8 bits for current ROM bank
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;
static int sram_modified = 0;

void setup_MBC5(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    if (battery) {
        read_SRAM();
    }
}

uint8_t read_MBC5(uint16_t addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: // Reading from fixed Bank 0
     case 0x3000: return ROM_banks[addr]; 
                  break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000: // Reading from current ROM bank 1 
                return ROM_banks[(((rom_bank_hi_bit << 8) | rom_bank_low) * ROM_BANK_SIZE) + (addr - 0x4000)];
                break;
        
     case 0xA000:
     case 0xB000: // Read from RAM bank (if RAM banking enabled)
                 return ram_banking ? RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr - 0xA000)] : 0xFF;
                 break;
    };
    // Failed to read
    return 0x0;
}


void write_MBC5(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking
                    if (battery && ram_banking && ((val & 0xF) != 0xA) && sram_modified) {
                        write_SRAM();
                        sram_modified = 0;
                    }
                    ram_banking = ((val & 0xF) == 0xA);
                    break;
        case 0x2000: // Set lower 8 bits of ROM bank */
                    rom_bank_low = val;
                    break;
        case 0x3000:// Set 9th bit of ROM bank
                    rom_bank_hi_bit = val & 0x1;
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank 0 - F
                     cur_RAM_bank = (val & 0xF) & (RAM_bank_count - 1);
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (ram_banking) {
                        RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr - 0xA000)] = val; 
                        sram_modified = 1;
                    }
                    break;
    }    
}
