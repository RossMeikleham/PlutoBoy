#include "mbc.h"
#include "mmm01.h"
#include "memory.h"

/* Memory Bank Controller MMM01
 *
 * 3 different MMM01 controller types:
 * (1) :ROM ONLY : Can only write/read from ROM banks
 * (2) :ROM + RAM : Can write/read from RAM Banks
 * (3): ROM + RAM + SAVE : Same as (2) but saves to SRAM
 * 
 * Currently works with Momotarous Collection 2, but
 * I need more documentation to be able to implement
 * this more correctly so it works with Taito Pack
*/

static int rom_mode = 0; 
static int rom_select = 1; // Current ROM bank 0x0 - 0x1F
static int ram_select = 0; // Current RAM bank 0x0 - 0x03
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;
static int rom_base = 0;

void setup_MMM01(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    // Check for previous saves if Battery active
    if (battery) {
        read_SRAM();
    }
}


uint8_t read_MMM01(uint16_t addr) {
    if ((addr & 0x8000) == 0x0000) {
        if (rom_mode == 0) {
            return ROM_banks[addr];
        }
     }

     if ((addr & 0xC000) == 0x0000) {
        return ROM_banks[((rom_base + 2) * ROM_BANK_SIZE) | addr]; //1st 2 banks used by game menu
     }

     if ((addr & 0xC000) == 0x4000) {
        return ROM_banks[((1 + rom_base + rom_select) * ROM_BANK_SIZE) + addr];
     }

     if ((addr & 0xE000) == 0xA000) {
        if (ram_banking) {
            return RAM_banks[(ram_select * RAM_BANK_SIZE) | (addr & 0x1FFF)];   
        }
     }

     return 0x00;
}




void write_MMM01(uint16_t addr, uint8_t val) {
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000:
                    if (rom_mode == 0) {
                        rom_mode = 1;
                    } else {
                        // Activate/Deactivate RAM banking
                        // If deactivating, save to file
                        if (battery && ram_banking && ((val & 0xF) != 0xA)) {
                            write_SRAM();
                        }
                        ram_banking = ((val & 0xF) == 0xA);
                    }
                    break;
        case 0x2000:
        case 0x3000:
                    if (rom_mode == 0) {
                        rom_base = val & 0x3F;
                    } else {
                        rom_select = val;
                    }
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank
                     if (rom_mode == 1) {
                        ram_select = val;
                     }
                     break;
        case 0x6000: 
        case 0x7000: //Unknown purpose 
                     break;

        case 0xA000:
        case 0xB000: // Write to RAM bank if RAM banking enabled 
                    if (ram_banking) {
                        RAM_banks[(ram_select * RAM_BANK_SIZE) | (addr - 0xA000)] = val; 
                    }
                    break;
    }    
}
