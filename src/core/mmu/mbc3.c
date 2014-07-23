#include "mbc.h"
#include "mbc3.h"
#include "memory.h"

typedef enum {RTC, RAM} RTC_RAM_mode; 

static RTC_RAM_mode rtc_ram_mode = RAM;
static int bank_mode = 0;
static int current_RAM_bank = 0;
static int current_RTC_reg = 0;
static int current_ROM_bank = 1;
static int ram_banking = 1;
static int real_time_clock = 0;
static int last_latch = 0xF;

uint8_t read_MBC3(uint16_t addr) {
        
        if (addr < 0x4000) {
            return ROM_banks[0][addr];
        }
        //Read using ROM Banking 0x4000 - 0x7FFF
        if((addr & 0xC000) == 0x4000) {
            return ROM_banks[current_ROM_bank][addr - 0x4000];
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


void write_MBC3(uint16_t addr, uint8_t val) {
    
    // Setting ROM/RAM banking mode
    if(addr <= 0x1FFF) {
        ram_banking = (val & 0xF) == 0xA;
    // Setting ROM bank
    } else if((addr >= 0x2000) && (addr <= 0x3FFF)) {
         //printf("switching rom bank %d\n", current_ROM_bank); 
         current_ROM_bank = (val & 0x7F) + ((val & 0x7F) == 0);
       
    } else if((addr >= 0x4000) && (addr <= 0x5FFF)) { 
        uint8_t nibble = val & 0xF;
        if (nibble <= 0x3) {
            // Set RAM bank
            rtc_ram_mode = RAM;
            current_RAM_bank = nibble;

        } else if (nibble >= 0x8 && nibble <= 0xC) {
            // Set Real Time Clock register
            rtc_ram_mode = RTC;
            current_RTC_reg = nibble - 0x8;
        }
        current_RAM_bank = (val & 0x3); 

    } else if((addr >= 0x6000) && (addr <= 0x7FFF)) { 
        if (rtc_ram_mode == RTC) { // Latch RTC reg
            if (last_latch == 0x0 && (val = 0x1)) {
                //TODO write real time to current RTC reg
            }   
            last_latch = val; 
       }
     
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
