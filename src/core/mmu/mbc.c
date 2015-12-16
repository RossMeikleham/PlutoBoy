#include "mbc.h"
#include "mbc0.h"
#include "mbc1.h"
#include "mbc3.h"
#include "mbc5.h"
#include "mmm01.h"

#include "../../non_core/logger.h"
#include "../../non_core/files.h"

#include "string.h"

#define MAX_SRAM_FNAME_SIZE 256

static char SRAM_filename[MAX_SRAM_FNAME_SIZE + 1];
static unsigned RAM_bank_count = 0;

void write_SRAM() {
    save_SRAM(SRAM_filename, RAM_banks[0], RAM_bank_count * 0x2000);
}


void read_SRAM() {

    size_t len;
    if((len = load_SRAM(SRAM_filename, RAM_banks[0], RAM_bank_count * 0x2000))) {
        if (len !=( RAM_bank_count * 0x2000)) { // Not enough read in
            memset(RAM_banks[0],0,len); //"Erase" what just got read into memory
        }
    }
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
    
    strncpy(SRAM_filename, filename, end_pos);
    strcat(SRAM_filename, extension); 
}


int setup_MBC(int MBC_no, unsigned ram_banks, const char *filename) {

    create_SRAM_filename(filename);
    RAM_bank_count = ram_banks;

    int flags = 0;
    //MMBC0
    if (MBC_no == 0) {

        read_MBC = &read_MBC0;
        write_MBC = &write_MBC0;  

    //MBC1
   } else if(MBC_no >= 1 && MBC_no <= 3) {
        switch (MBC_no) {
          case  3:flags = SRAM | BATTERY; break;
          case  2:flags = SRAM; break;
          case  1:flags = 0; break;
        }

        setup_MBC1(flags);
        read_MBC = &read_MBC1;
        write_MBC = &write_MBC1;

   //MMM01
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
            case 0xF : flags = RTC | BATTERY; break;
            case 0x10: flags = RTC | BATTERY | SRAM; break;
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
           case 0x19: flags = 0;
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
   else{ 
       log_message(LOG_ERROR, "unimplimented MBC mode for code %d\n",MBC_no);
       return 0;
   }

   return 1;
}



