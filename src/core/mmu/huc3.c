
#include "mbc.h"
#include "huc3.h"
#include "memory.h"
#include "../bits.h"
#include "../../non_core/get_time.h"


/* Hudson Memory Bank Controller 3 (2MB ROM, 128KB RAM, RTC)
 * 
 * Uses a 28-bit Clock
 *
 * 0000-3FFF	ROM Bank 0 (16KB)
 * 4000-7FFF	ROM Bank 1-127 (16KB)
 * A000-BFFF	RAM Bank 0-15 (8KB)
 */

static int cur_RAM_bank = 0; // Current ROM bank 0x0 - 0x7F
static int cur_ROM_bank = 1; // Current RAM bank 0x0 - 0x0F
static int ram_banking = 0;  // 0: RAM banking off, 1: RAM banking on
static int battery = 0;
static int huc3_ramflag = 0;
static int huc3_value = 0;
static uint64_t clock_register = 0;
static uint64_t clock_shift = 0;
static uint64_t clock_time = 0;


void setup_HUC3(int flags) {
    battery = (flags & BATTERY) ? 1 : 0;
    // Check for previous saves if Battery active
    if (battery) {
        read_SRAM();
    }
}



void update_clock() {
	uint64_t now = 0; // get_time();
    uint64_t elapsed = now - clock_time;
	
	// Get Years 4 bits
	while (elapsed >= 365 * 24 * 60 * 60) {
		clock_register += (1 << 24);
		elapsed -= 365 * 24 * 60 * 60;
	}	

	// Get Days 12 bits
	while (elapsed >= 24 * 60 * 60) {
		clock_register += (1 << 12);
		elapsed -= 24 * 60 * 60;
	}

	// Minutes 12 bits
	while (elapsed >= 60) {
		clock_register += 1;
		elapsed -= 60;
	}

	if ((clock_register & 0x0000FFF) >= 24 * 60) {
		clock_register += (1 << 12) - 24 * 60;
	}
	
	if ((clock_register & 0x0FFF000) >= 365 << 12) {
		clock_register += (1 << 24) - (365 << 12);
	}

	clock_time = now - elapsed;
}
	




uint8_t read_HUC3(uint16_t addr) {
       
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
				 switch (huc3_ramflag) {
				   case 0x0: 
				   case 0xA: if (ram_banking) {
                    			return RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr & 0x1FFF)];
							 } 
							 break;
		
				   case 0xC: return huc3_value; break;
				   case 0xD: return 0x1; break;
				   default : return 0xFF;
				   
				 }
    };
    // Failed to read
    return 0xFF;
}


void write_HUC3(uint16_t addr, uint8_t val) {
    
    switch (addr & 0xF000) {
        case 0x0000:
        case 0x1000: 
					ram_banking = val & 0x0A;
                    huc3_ramflag = val;
                    break;
        case 0x2000:
        case 0x3000:/* Set ROM bank  */
                    val = val & 0x7F;
                    cur_ROM_bank = val ? val : 1;
					
                    break;
        case 0x4000: 
        case 0x5000: // Set current RAM bank 0 - 0xF
                     cur_RAM_bank = val & 0xF;
                     break;
        case 0x6000: 
        case 0x7000: //Nothing
                     break;
        case 0xA000:
        case 0xB000: // Write to external RAM bank if RAM banking enabled 
                    if (huc3_ramflag == 0xB) {
						switch (val & 0xF0) {
							case 0x10:  
									if (clock_shift <= 24) {
										huc3_value = (clock_register >> clock_shift) & 0x0F;
										clock_shift += 4;
									}
									break;

							case 0x30: 				 
									if (clock_shift <= 24) {
										clock_register &= ~(0x0F << clock_shift);
										clock_register |= ((val & 0xF) << clock_shift);
										clock_shift += 4;
									}
									break;

							case 0x40:
									update_clock();
							
									switch (val & 0xF) {
							    		case 0x00:
										case 0x03:
										case 0x07: 
										   	clock_shift = 0;
										   	break;	
									}
									break;
						
							case 0x60:
									huc3_value = 0x1; 								
									break;	
						}	
					} else if (huc3_ramflag == 0x0A && ram_banking) {
							RAM_banks[(cur_RAM_bank * RAM_BANK_SIZE) | (addr & 0x1FFF)] = val;					
					} 
                    break;
    }    
}
