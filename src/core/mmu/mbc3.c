#include "mbc.h"
#include "mbc3.h"
#include "memory.h"
#include "../bits.h"

#include  <time.h>

static int cur_RAM_bank = 0;
static int cur_ROM_bank = 1;
static int ram_enabled = 0;
static int last_latch = 0;

static int battery = 0;
static int rtc_enabled = 0;

static rtc_regs_MBC3 rtc_regs, latch_regs;


void inc_rtc_second() {
  if ((rtc_regs.flags & BIT_6) == 0) {

  	if(++rtc_regs.seconds >= 60) {
      rtc_regs.seconds = 0;

      if(++rtc_regs.minutes >= 60) {
        rtc_regs.minutes = 0;

        if(++rtc_regs.hours >= 24) {
          rtc_regs.hours = 0;

		  if (++rtc_regs.days_low == 0) {
		  	if (rtc_regs.flags & 0x1) {
				rtc_regs.flags |= 0x7;
			} else {
				rtc_regs.flags |= 0x1;
			}	
          }
        }
      }
    }
  }
}




void setup_MBC3(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    rtc_enabled = (flags & RTC) ? 1 : 0;

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
                if (ram_enabled && cur_RAM_bank <= 0x3) {
                   return RAM_banks[cur_RAM_bank][addr - 0xA000];
                
				// Read from RTC register
				} else {
				  switch (cur_RAM_bank) {
					case 0x8: return latch_regs.seconds; break;
					case 0x9: return latch_regs.minutes; break;
					case 0xA: return latch_regs.hours; break;
					case 0xB: return latch_regs.days_low; break;
					case 0xC: return latch_regs.flags; break;	
				  }
				}
                break;
    };
    // Failed to read
    return 0x0;
}


void write_MBC3(uint16_t addr, uint8_t val) {

    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: // Activate/Deactivate RAM banking/RTC
                    // From ON to OFF, save SRAM to file
                    if (battery && ram_enabled && ((val & 0xF) != 0xA)) {
                        write_SRAM();
                    }
                    ram_enabled = ((val & 0xF) == 0xA);
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank, if result is 0,
                     * increment the bank as it cannot be used */
                    cur_ROM_bank = (val & 0x7F) + ((val & 0x7F) == 0);
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM/RTC mode and banks
                    cur_RAM_bank =  val;
                    break;
        case 0x6000: 
        case 0x7000: //Latch to RTC reg if 0x0 followed by 0x1 written
                    if (ram_enabled && rtc_enabled) { 
                        if (last_latch == 0x0 && (val == 0x1)) {
							 latch_regs = rtc_regs;
                        }   
                        last_latch = val; 
                     }
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (ram_enabled && cur_RAM_bank <= 0x3) {
                        RAM_banks[cur_RAM_bank][addr - 0xA000] = val; 
                    // Write to RTC
                    } else if (ram_enabled && rtc_enabled) {
                    	switch (cur_RAM_bank) {
							case 0x8: rtc_regs.seconds = val; break;
							case 0x9: rtc_regs.minutes = val; break;
							case 0xA: rtc_regs.hours = val; break;
							case 0xB: rtc_regs.days_low = val; break;
							case 0xC: rtc_regs.flags = val; break;	
						}
                    }
                    break;
    }    
}
