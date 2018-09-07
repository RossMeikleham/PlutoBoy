#include "mbc.h"
#include "huc1.h"
#include "memory.h"

/* Hudson Memory Controller 1 
 *
 * Contains ROM + RAM + SAVE
*/

static int cur_RAM_bank = 0; // Current ROM bank 0x0 - 0x1F
static int cur_ROM_bank = 1; // Current RAM bank 0x0 - 0x03
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;

void setup_HUC1(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    // Check for previous saves if Battery active
    if (battery) {
        read_SRAM();
    }
}


uint8_t read_HUC1(uint16_t addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: // Reading from fixed Bank 0
     case 0x3000: return ROM_banks[addr]; break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000:
                 return ROM_banks[(cur_ROM_bank * ROM_BANK_SIZE) | (addr & 0x3FFF)]; 
                 break;
        
     case 0xA000:
     case 0xB000:
                 return RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr & 0x1FFF)]; 
                 break;
    };
    // Failed to read
    return 0x0;
}


void write_HUC1(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking
                    // If deactivating, save to file
                    if (battery && ram_banking && ((val & 0xF) != 0xA)) {
                        write_SRAM();
                    }
                    ram_banking = ((val & 0xF) == 0xA);
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank, can't be 0 */
                    cur_ROM_bank = val == 0 ? 1 : val;
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank 0 - 3
                     cur_RAM_bank = val;
                     break;
        case 0x6000: 
        case 0x7000: //Nothing
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (ram_banking) {
                        RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr & 0x1FFF)] = val; 
                    }
                    break;
    }    
}
