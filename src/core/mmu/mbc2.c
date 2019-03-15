#include "mbc.h"
#include "mbc2.h"
#include "memory.h"

/* Memory Bank Controller 2
 *
 * 256KB ROM, 256 Bytes RAM
 * 
 * Ram can only write 4 bits instead of 8
 *
 * 2 different MBC2 controller types:
 * (1) :ROM + RAM : Can write/read from RAM Banks
 * (2): ROM + RAM + SAVE : Same as (2) but saves to SRAM
*/

static int cur_ROM_bank = 1; // Current ROM bank 0x0 - 0x0F
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;

void setup_MBC2(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    // Check for previous saves if Battery active
    if (battery) {
        read_SRAM();
    }
}


uint8_t read_MBC2(uint16_t addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: // Reading from fixed Bank 0
     case 0x3000: return ROM_banks[addr]; break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000: // Reading from current ROM bank 1 
                  return ROM_banks[(cur_ROM_bank * ROM_BANK_SIZE) | (addr & 0x3FFF)];
                  break;
        
     case 0xA000:
                 // Read from RAM bank (if RAM banking enabled)
                if (ram_banking) {
                    return RAM_banks[addr & 0x1FF];
                } break;
    };
    // Failed to read
    return 0x0;
}


void write_MBC2(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking
                    // If deactivating, save to file,
                    if ((addr & 0x100) == 0x0) {
                        if (battery && ram_banking && ((val & 0xF) != 0xA)) {
                            write_SRAM();
                        }
                        ram_banking = ((val & 0xF) == 0xA);
                    }
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank */
                    if ((addr & 0x100) != 0x0) {
                        cur_ROM_bank = (val & 0xF) + ((val & 0xF) == 0);
                    }
                    break;
        
        case 0xA000: // Write to RAM bank if RAM banking enabled
                    if (ram_banking) {
                        RAM_banks[addr & 0x1FF] = val & 0xF; 
                    }
                    break;
    }    
}
