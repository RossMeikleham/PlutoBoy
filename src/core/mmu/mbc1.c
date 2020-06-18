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

static int bank_mode = 0; // 0: 2MB ROM mode, 1: 512KB ROM mode
static int cur_RAM_bank_num = 0; // Current RAM bank 0x0 - 0x03
static int cur_ROM_bank_num = 1; // Current ROM bank 0x0 - 0x1F
static uint8_t *cur_ROM_bank_0 = 0; // 0x0000 -> 0x3FFF mapping
static uint8_t *cur_ROM_bank = 0x00; // Full bank to access, can be 0x00 -> 0x7F * bank size
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;


static void set_cur_ROM_bank()
{
    int full_bank = cur_ROM_bank_num;
    int full_rom_bank_0 = 0;
        
    full_bank |= (cur_RAM_bank_num << 5);
    if (bank_mode == 1) // bank_mode 1
    {
        full_rom_bank_0 = cur_RAM_bank_num << 5;
    }
     
    full_bank %= ROM_bank_count;
    cur_ROM_bank = &ROM_banks[(full_bank * ROM_BANK_SIZE) - 0x4000];

    full_rom_bank_0 %= ROM_bank_count;
    cur_ROM_bank_0 = &ROM_banks[full_rom_bank_0 * ROM_BANK_SIZE];
}


void setup_MBC1(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    // Check for previous saves if Battery active
    if (battery) {
        read_SRAM();
    }

    set_cur_ROM_bank();
}


uint8_t read_MBC1(uint16_t addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: // Reading from fixed Bank 0
     case 0x3000: return cur_ROM_bank_0[addr]; break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000: // Reading from current ROM bank 1 
              //  printf("cur rom bank %d cur_ram bank %d\n",cur_ROM_bank, cur_RAM_bank);
                return cur_ROM_bank[addr];
     case 0xA000:
     case 0xB000: // Read from RAM bank (if RAM banking enabled)
                if (ram_banking) {
                   
                   uint8_t res = bank_mode == 0 ? 
                        RAM_banks[addr - 0xA000] :
                         
                        RAM_banks[((cur_RAM_bank_num % RAM_bank_count) * RAM_BANK_SIZE) | (addr - 0xA000)];
                   return res;
                } break;
    };
    // Failed to read
    return 0xFF;
}



void write_MBC1(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking
                    // If deactivating, save to file
                    if (battery && ram_banking && ((val & 0xF) != 0xA)) {
                        write_SRAM();
                    }
                    
                    if (RAM_bank_count > 0) {
                        ram_banking = ((val & 0xF) == 0xA);
		            }
                    break;
        case 0x2000:
        case 0x3000:/* Inc ROM bank as it can't be 0*/
                    cur_ROM_bank_num = (val & 0x1F);
                    if (cur_ROM_bank_num == 0)
                    {
                        cur_ROM_bank_num = 1;
                    }
                    set_cur_ROM_bank();
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank 0 - 3
                     cur_RAM_bank_num = (val & 0x3);
                     set_cur_ROM_bank();
                     break;
        case 0x6000: 
        case 0x7000: //Change between 2MB RAM/8KB ROM and 512KB RAM/32KB ROM modes 
                     bank_mode = (val & 0x1);
                     if (bank_mode == 0)
                     {
                         //cur_RAM_bank_num = 0;
                         cur_ROM_bank_num = 1;
                     }
                     set_cur_ROM_bank();
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (ram_banking) {
                        int bank = (bank_mode == 0) ? 0 : (cur_RAM_bank_num % RAM_bank_count) ;
                        RAM_banks[(bank * RAM_BANK_SIZE) | (addr - 0xA000)]= val; 
                    }
                    break;
    }    
}
