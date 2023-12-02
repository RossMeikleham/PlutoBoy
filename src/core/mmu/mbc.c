#include "mbc.h"
#include "mbc0.h"
#include "mbc1.h"
#include "mbc2.h"
#include "mbc3.h"
#include "mbc5.h"
#include "mmm01.h"
#include "huc1.h"
#include "huc3.h"

#include "../../non_core/logger.h"
#include "../../non_core/files.h"
#include "../../non_core/get_time.h"

#include <string.h>
#include <stdlib.h>

#ifdef EFIAPI
#include "../platforms/UEFI/libs.h"
#define PB_STRNCPY uefi_strncpy
#define PB_STRCAT uefi_strcat
#else
#define PB_STRNCPY strncpy
#define PB_STRCAT strcat
#endif

uint8_t *RAM_banks; // max 16 * 8KB ram banks (128KB) 0x2000
uint8_t *ROM_banks; // max 512 * 16KB rom banks (8MB) 0x4000

read_MBC_ptr read_MBC = NULL;
write_MBC_ptr write_MBC = NULL; 

#define MAX_SRAM_FNAME_SIZE 256

static char SRAM_filename[MAX_SRAM_FNAME_SIZE + 1];
unsigned RAM_bank_count = 0;
unsigned ROM_bank_count = 0;
static int mbc3_rtc = 0;

// SRAM cache is where we store any writes to the SRAM, we only write to the SRAM file every SRAM_WRITE_DELAY milliseconds
// used to improve 3DS performance and avoid taxing I/O writes
static unsigned char *SRAM_cache; // max 16 * 8KB ram banks (128KB) 0x2000
static uint64_t time_last_SRAM_write = 0;   // in ms
static int SRAM_cache_valid = 0;    // 0 = false, 1 = true, is the cache safe to use for loading

void write_SRAM() {

    log_message(LOG_INFO, "write SRAM called\n");
    // if sufficent time has passed since last file write, write to the .sav file
    if(get_time() - time_last_SRAM_write > SRAM_WRITE_DELAY){
        save_SRAM(SRAM_filename, RAM_banks, RAM_bank_count * 0x2000);
    }
    // always save to cache that way any load SRAM calls can use the cache
    save_SRAM_cached(SRAM_cache, RAM_banks, RAM_bank_count * 0x2000);

    time_last_SRAM_write = get_time();
    SRAM_cache_valid = 1;   // since we always ensure the cache matches the file, it's always valid except on load up
}


void flush_SRAM() {
    save_SRAM(SRAM_filename, RAM_banks, RAM_bank_count * 0x2000);
}


void read_SRAM() {

    size_t len;
    if(SRAM_cache_valid){  // depending on if the cache is loaded, use cache or the file
        len = load_SRAM_cached(SRAM_cache, RAM_banks, RAM_bank_count * 0x2000);
    }
    else{
        len = load_SRAM(SRAM_filename, RAM_banks, RAM_bank_count * 0x2000);
    }

    if(len) {
        if (len !=( RAM_bank_count * 0x2000)) { // Not enough read in
            memset(RAM_banks, 0, len); //"Erase" what just got read into memory
        }
    }
}



void inc_sec_mbc3() {
	if (mbc3_rtc) {
		inc_rtc_second();
	}
}

//TODO
rtc_regs_MBC3 read_RTC() {
  	rtc_regs_MBC3 r;

	r.seconds = 0x0;
	r.minutes = 0x0;
	r.hours = 0x0;
	r.days_low = 0x0;
	r.flags = 0x0;
	
	return r;	
}

//TODO
void write_RTC(rtc_regs_MBC3 rtc_regs){
	return;
}


static void create_SRAM_filename(const char *filename) {

    memset(SRAM_filename, 0, MAX_SRAM_FNAME_SIZE);
    char *extension = ".sav";

    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(extension);

    /* If filename is too large then replace last characters with sav extension
     * otherwise append file extension onto filename */
    size_t end_pos = filename_len + extension_len <= MAX_SRAM_FNAME_SIZE ? 
           filename_len : 
           MAX_SRAM_FNAME_SIZE - extension_len;
    
    PB_STRNCPY(SRAM_filename, filename, end_pos);
    PB_STRCAT(SRAM_filename, extension); 
}


void teardown_MBC() {
   free(RAM_banks); 
   free(ROM_banks); 
   free(SRAM_cache);
}

int setup_MBC(int MBC_no, unsigned ram_banks, unsigned rom_banks, const char *filename) {

    create_SRAM_filename(filename);
    RAM_bank_count = ram_banks;

	RAM_banks = NULL;
	if (RAM_bank_count > 0) {
    	RAM_banks = malloc(ram_banks * RAM_BANK_SIZE);
    	if (RAM_banks == NULL) {
        	log_message(LOG_ERROR, "Unable to allocate memory for RAM banks\n");
        	return 0;
    	}

        SRAM_cache = malloc(ram_banks * RAM_BANK_SIZE);
    	if (SRAM_cache == NULL) {
        	log_message(LOG_ERROR, "Unable to allocate memory for SRAM cache\n");
        	return 0;
    	}
	}

    ROM_banks = malloc(rom_banks * ROM_BANK_SIZE);
    if (ROM_banks == NULL) {
        log_message(LOG_ERROR, "Unable to allocate memory for ROM banks\n");
        if (RAM_banks != NULL) {
			free(RAM_banks);
		}
        if (SRAM_cache != NULL) {
			free(SRAM_cache);
		}
        return 0;
    }
    ROM_bank_count = rom_banks;

    int flags = 0;
    // MMBC0
    if (MBC_no == 0) {

        read_MBC = &read_MBC0;
        write_MBC = &write_MBC0;  

   // MBC1
   } else if(MBC_no >= 1 && MBC_no <= 3) {
        switch (MBC_no) {
          case  3:flags = SRAM | BATTERY; break;
          case  2:flags = SRAM; break;
          case  1:flags = 0; break;
        }

        setup_MBC1(flags);
        read_MBC = &read_MBC1;
        write_MBC = &write_MBC1;

   // MBC2
   } else if (MBC_no >= 5 && MBC_no <= 6) {
   		switch (MBC_no) {
			case 0x5: flags = SRAM; break;
			case 0x6: flags = SRAM | BATTERY; break;
		}
		setup_MBC2(flags);
		read_MBC = &read_MBC2;
		write_MBC = &write_MBC2;

   // MMM01
   } else if(MBC_no >= 0xB && MBC_no <= 0xD) {
        
        switch (MBC_no) {
            case 0xB: flags = 0; break;
            case 0xC: flags = SRAM; break;
            case 0xD: flags = SRAM | BATTERY; break;
        } 
        
        setup_MMM01(flags);
        read_MBC =&read_MMM01;
        write_MBC = &write_MMM01; 

   // MBC3
   } else if(MBC_no >= 0xF && MBC_no <= 0x13) {
   
        switch (MBC_no) {
            case 0xF : flags = RTC | BATTERY; mbc3_rtc = 1; break;
            case 0x10: flags = RTC | BATTERY | SRAM; mbc3_rtc = 1; break;
            case 0x11: flags = 0; break;
            case 0x12: flags = SRAM; break;
            case 0x13: flags = BATTERY | SRAM; break;
        } 

        setup_MBC3(flags);
        read_MBC = &read_MBC3;
        write_MBC = &write_MBC3;
    
   // MBC5 
   } else if (MBC_no >= 0x19 && MBC_no <= 0x1E) {

       switch (MBC_no) {
           case 0x19: flags = 0; break;
           case 0x1A: flags = SRAM; break;
           case 0x1B: flags = SRAM | BATTERY; break;
           case 0x1C: flags = RUMBLE; break;
           case 0x1D: flags = RUMBLE | SRAM; break;
           case 0x1E: flags = RUMBLE | SRAM | BATTERY; break;          
       }
        
        setup_MBC5(flags);
        read_MBC = &read_MBC5;
        write_MBC = &write_MBC5;
   }
  
   // HUC3
   else if (MBC_no == 0xFE) {
       flags = SRAM | BATTERY | RTC;
       setup_HUC3(flags);
       read_MBC = &read_HUC3;
       write_MBC = &write_HUC3;
   } 
   
   // HUC1
   else if (MBC_no == 0xFF) {
        flags = SRAM | BATTERY;
        setup_HUC1(flags);
        read_MBC = &read_HUC1;
        write_MBC = &write_HUC1;
   }
    
   else{ 
       log_message(LOG_ERROR, "unimplimented MBC mode for code %d\n",MBC_no);
       return 0;
   }

   return 1;
}
