#include "mbc.h"
#include "mbc3.h"
#include "memory.h"


static int rtc_ram_mode = SRAM;
static int cur_RAM_bank = 0;
static int current_RTC_reg = 0;
static int cur_ROM_bank = 1;
static int rtc_ram_enabled = 0;
static int last_latch = 0xF;

static int battery = 0;
static int rtc = 0;

void setup_MBC3(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    rtc = (flags & RTC) ? 1 : 0;

    if (battery) {
        read_SRAM();
    }
}

uint8_t read_MBC3(uint16_t addr) {
       
    switch (addr & 0xF000) {
     case 0x0000:
     case 0x1000:
     case 0x2000: 
     case 0x3000: // Reading from fixed Bank 0 
                return ROM_banks[0][addr]; 
                break;
        
     case 0x4000:
     case 0x5000:
     case 0x6000:
     case 0x7000: // Reading from current ROM bank 1 
                return ROM_banks[cur_ROM_bank][addr - 0x4000];
                break;
        
     case 0xA000:
     case 0xB000: // Read from RAM bank (if RAM banking enabled)
                if (rtc_ram_enabled && rtc_ram_mode == SRAM) {
                   return RAM_banks[cur_RAM_bank][addr - 0xA000];
                } //else TODO RTC read
                break;
    };
    // Failed to read
    return 0x0;
}


void write_MBC3(uint16_t addr, uint8_t val) {
    uint8_t nibble;

    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking/RTC
                    // From ON to OFF, save SRAM to file
                    if (battery && rtc_ram_enabled && 
                        rtc_ram_mode == SRAM && ((val & 0xF) == 0x0)) {

                        write_SRAM();
                    }
                    rtc_ram_enabled = ((val & 0xF) == 0xA);
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank, if result is 0,
                     * increment the bank as it cannot be used */
                    cur_ROM_bank = (val & 0x7F) + ((val & 0x7F) == 0);
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM/RTC mode and banks
                    nibble =  val & 0xF;
                    if (nibble <= 0x3) {
                        // Set RAM bank
                        rtc_ram_mode = SRAM;
                        cur_RAM_bank = nibble;
                    // Nibble between 0x8 and 0xC
                    } else if ((uint8_t)(nibble - 0x8) < 0x4)  {
                        // Set Real Time Clock register
                        rtc_ram_mode = RTC;
                        current_RTC_reg = nibble - 0x8;
                     }       
                     break;
        case 0x6000: 
        case 0x7000: //Latch to RTC reg if 0x0 followed by 0x1 written
                    if (rtc_ram_enabled && rtc_ram_mode == RTC) { 
                        if (last_latch == 0x0 && (val = 0x1)) {
                            //TODO write real time to current RTC reg
                        }   
                        last_latch = val; 
                     }
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (rtc_ram_enabled && rtc_ram_mode == SRAM) {
                        RAM_banks[cur_RAM_bank][addr - 0xA000] = val; 
                    // Write to RTC
                    } else if (rtc_ram_enabled && rtc_ram_mode == RTC) {
                        //TODO RTC write
                    }
                    break;
    }    
}
