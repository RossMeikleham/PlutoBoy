#include "mbc.h"
#include "mbc1.h"
#include "memory.h"

static int bank_mode = 1;
static int current_RAM_bank = 0;
static int current_ROM_bank = 1;
static int ram_banking;

static MBC1_MODE mode;

void setup_MBC1(MBC1_MODE m) {
    mode = m;
    ram_banking = !(m == ROM_ONLY); 
}


uint8_t read_MBC1(uint16_t addr) {
        
        if (addr < 0x4000) {
            return ROM_banks[0][addr];
        }
        //Read using ROM Banking 0x4000 - 0x7FFF
        if((addr & 0xC000) == 0x4000) {
            if (bank_mode == 0) { //Upper 2 bits of ROM bank speciffied by ram bits 
                return ROM_banks[(current_RAM_bank << 5) | 
                    current_ROM_bank][addr - 0x4000];
            } else {
                return ROM_banks[current_ROM_bank][addr - 0x4000];
            }
        }

        //Read using RAM Banking 0xA000 - 0xBFFF
        else if(((addr & 0xE000) == 0xA000) && ram_banking) {
            if (bank_mode == 0) {
                return RAM_banks[0][addr - 0xA000]; 
             } else { 
                return RAM_banks[current_RAM_bank][addr - 0xA000]; 
             } 
        }

        return 0x0;
}


void write_MBC1(uint16_t addr, uint8_t val) {
    
    // Setting ROM/RAM banking mode
    if(addr <= 0x1FFF) {
        ram_banking = (val & 0xF) == 0xA;
    // Setting ROM bank
    } else if((addr >= 0x2000) && (addr <= 0x3FFF)) {

         current_ROM_bank = (val & 0x1F) + ((val & 0x1F) == 0);
       //  if current_ROM_bank = 0x0 0x20 0x40 0x60 
       
    } else if((addr >= 0x4000) && (addr <= 0x5FFF)) { 
        current_RAM_bank = (val & 0x3); 
    } else if((addr >= 0x6000) && (addr <= 0x7FFF)) { 
       // printf("switching bank mode: val %d new mode %d\n",val, val & 0x1);
        bank_mode = (val & 0x1); 
    } 
    //Write to External RAM (0xA000 - 0xBFFF)
     else if(((addr & 0xE000) == 0xA000)  && ram_banking) { 
        if(bank_mode == 0) { 
            RAM_banks[0][addr - 0xA000] = val; 
        } else {
            RAM_banks[current_RAM_bank][addr - 0xA000] = val; 
            }
        }
}
