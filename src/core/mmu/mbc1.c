#include "mbc.h"
#include "mbc1.h"
#include "memory.h"

/* Memory Bank Controller 1 
 * 2 modes which can be swapped between: 
 * (1) 2MB ROM and 8KB RAM RAM 
 * (2)  512KB ROM and 32KB RAM
 *
 * Banks 0x0 0x20 0x40 and 0x60 cannot be set, if attempting
 * to the next bank is set instead (i.e. 0x1 0x21 0x41 0x61)
 *
 * 3 different MBC1 controller types:
 * (1) :ROM ONLY : Can only write/read from ROM banks
 * (2) :ROM + RAM : Can write/read from RAM Banks
 * (3): ROM + RAM + SAVE : Same as (2) but saves to SRAM
*/

static int bank_mode = 1;
static int cur_RAM_bank = 0;
static int cur_ROM_bank = 1;
static int ram_banking = 0;

static MBC1_MODE mode;

void setup_MBC1(MBC1_MODE m) {
    mode = m;
}


uint8_t read_MBC1(uint16_t const addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: // Reading from fixed Bank 0
     case 0x3000: return ROM_banks[0][addr]; break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000: // Reading from current ROM bank 1 
                return bank_mode == 0 ?
                    ROM_banks[(cur_RAM_bank << 5) | cur_ROM_bank][addr - 0x4000] :
                    ROM_banks[cur_ROM_bank][addr - 0x4000];
                break;
        
     case 0xA000:
     case 0xB000: // Read from RAM bank (if RAM banking enabled)
                if (ram_banking) {
                   return bank_mode == 0 ? 
                        RAM_banks[0][addr - 0xA000] : 
                        RAM_banks[cur_RAM_bank][addr - 0xA000];
                } break;
    };
    // Failed to read
    return 0x0;
}


void write_MBC1(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking
                    ram_banking = ((val & 0xF) == 0xA);
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank, if result is 0, 0x20, 0x40, 0x60
                     * increment the bank as those cannot be used */
                    cur_ROM_bank = (val & 0x1F);
                    cur_ROM_bank += (cur_ROM_bank == 0x0) + 
                    (cur_ROM_bank == 0x20) + (cur_ROM_bank == 0x40) +
                    (cur_ROM_bank == 0x60);
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank 0 - 3
                     cur_RAM_bank = (val & 0x3);
                     break;
        case 0x6000: 
        case 0x7000: //Change between 2MB RAM/8KB ROM and 512KB RAM/32KB ROM modes 
                     bank_mode = (val & 0x1);
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (ram_banking) {
                        int bank = (bank_mode != 0) ? 0 : cur_RAM_bank;
                        RAM_banks[bank][addr - 0xA000] = val; 
                    }
                    break;
    }    
}
