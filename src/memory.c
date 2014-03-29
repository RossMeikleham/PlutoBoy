/*
 * =====================================================================================
 *
 *       Filename:  memory.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/03/14 19:54:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "memory.h"
#include "pthread.h"

#define LICENSES_LEN sizeof(licenses)/sizeof(licensee)
#define OLD_LICENSES_LEN sizeof(old_licenses)/sizeof(licensee)
#define ROM_SIZES_LEN sizeof(rom_sizes)/sizeof(rom_size)
#define CARTRIDGE_TYPE_LEN sizeof(cartridge_types)/sizeof(cartridge_type)  
#define RAM_SAVE_SIZES_LEN sizeof(ram_save_sizes)/sizeof(ram_save_size)


pthread_rwlock_t mem_lock = PTHREAD_RWLOCK_INITIALIZER;


licensee licenses[] = {{0, "none"}, {1, "nintendo"}, {8, "capcom"}, {13, "electronic arts"}, {18, "hudsonsoft"}, {19, "b-ai"},
     {20, "kss"}, {22, "pow"}, {24, "pcm complete"}, {25, "san-x"}, {28, "kemco japan"}, {29, "seta"},
     {30, "viacom"}, {31, "nintendo"}, {32, "bandai"}, {33, "ocean/acclaim"}, {34, "konami"}, {35, "hector"}, {37, "taito"},
     {38, "hudson"}, {39, "banpresto"}, {41, "ubi soft"}, {42, "atlus"}, {44, "malibu"}, {46, "angel"}, 
     {47, "pullet-proof"}, {49, "irem"}, {50, "absolute"}, {51, "acclaim"}, {52, "activision"},
     {53, "american sammy"}, {54, "gametek"}, {55, "park place"}, {56, "ljn"}}; 

licensee old_licenses[] = {{0, "none"}, {1, "nintendo"}, {8, "capcom"}, {13, "electronic arts"}, {18, "hudsonsoft"}, {19, "itc entertainment"},
      {24, "pcm complete"}, {25, "san-x"}, {28, "kotobuki systems"}, {29, "seta"},
     {30, "infogames"}, {31, "nintendo"}, {32, "bandai"}, {34, "konami"}, {35, "hector"}, 
     {38, "capcom"}, {39, "banpresto"}, {41, "ubi soft"}, {42, "atlus"}, {44, "malibu"}, {46, "angel"}, 
     {47, "spectrum holoby"}, {50, "absolute"}, {51, "acclaim"}, {52, "activision"},
     {53, "american sammy"}, {54, "konami"}, {55, "hi tech entertainment"}, {56, "ljn"}}; 


rom_size rom_sizes[] = {{0x0, 32}, {0x1, 64}, {0x2, 128}, {0x3, 256}, {0x4, 512},
    {0x5, 1024}, {0x6, 2048}, {0x7, 4096}, {0x52, 1152}, {0x53, 1280}, {0x54, 1536}};

cartridge_type cartridge_types[] = {{0x00 ,"ROM"}, {0x01, "ROM+MBC1"},
 {.id = 0x02, "ROM+MBC1+RAM"}, {0x03, "ROM+MBC1+RAM+BATTERY"}, {0x05, "ROM+MBC2"}, 
 {0x06, "ROM+MBC2+BATTERY"}, {0x08, "ROM+RAM"},
    {0x09, "ROM+RAM+BATTERY"}, {0x0B, "ROM+MMM01"}, {0x0C, "ROM+MMM01+SRAM"}, {0x0D, "ROM+MMM01+SRAM+BATTERY"},
    {0x0F, "ROM+MBC3+TIMER+BATTERY"}, {0x10, "ROM+MBC3+TIMER+RAM+BATTERY"}, {0x11, "ROM+MBC3"},
    {0x12, "ROM+MBC3+RAM"}, {0x13, "ROM+MBC3+RAM+BATTERY"}, {0x15, "ROM+MBC4"}, {0x16, "ROM+MBC4+RAM"},
    {0x17, "ROM+MBC4+RAM+BATTERY"}, {0x19, "ROM+MBC5"}, {0x1A, "ROM+MBC5+RAM"}, {0x1B, "ROM+MBC5+RAM+BATTERY"},
    {0x1C, "ROM+MBC5+RUMBLE"}, {0x1D, "ROM+MBC5+RUMBLE+SRAM"}, {0x1E, "ROM+MBC5+RUMBLE+SRAM+BATTERY"}, 
    {0xFC, "POCKET CAMERA"}, {0xFD, "BANDAI TAMA5"}, {0xFE, "HuC3"}, {0xFF, "HuC1+RAM+BATTERY"}};



ram_save_size ram_save_sizes[] = {{0, 0}, {1, 2}, {2, 8}, {3, 32}};

char *destination_codes[] = {"Japanese", "Non-Japanese"};

uint8_t mem[WORD_MAX];



void set_mem(uint16_t loc, uint8_t val) 
{
    pthread_rwlock_wrlock(&mem_lock);
    mem[loc] = val;
    pthread_rwlock_unlock(&mem_lock);
}

uint8_t get_mem(uint16_t loc) {

    uint8_t result;
    pthread_rwlock_rdlock(&mem_lock); 
    result = mem[loc];
    pthread_rwlock_unlock(&mem_lock);
    return result;
}




/*  Returns pointer to liscensee,
 *  Returns NULL if unknown id in loaded rom */
const char *get_licensee() {
   uint8_t i, id;

   /*  IF old licence code location contains 0x33
    *  the licence code is stored as 0xXY where 3X
    *  is the MSB of the new license code location
    *  and 3Y is the LSB of the new licence code location*/
   if (mem[OLD_LICENSE_CODE] == 0x33) {
       id = mem[NEW_LICENSE_CODE_MSB] << 0x4 |
       mem[NEW_LICENSE_CODE_LSB] & 0xF;
       for (i = 0; i < LICENSES_LEN; i++) {
           if(id == licenses[i].id) {
               return licenses[i].name;
           }
       }
   /* Otherwise licence code is stored at old licence location */
   } else {
       id = mem[OLD_LICENSE_CODE];
       for (i = 0; i < OLD_LICENSES_LEN; i++) {
           if (id == old_licenses[i].id) {
               return old_licenses[i].name;
           }
       }
   }
   return NULL;
   
}



/*  Returns rom size from supplied hex id, returns 0
 *  if unknown id supplied */
uint16_t get_rom_size(uint8_t id) {
    uint8_t i;
    for (i = 0; i < ROM_SIZES_LEN; i++) {
        if (id == rom_sizes[i].id) {
            return rom_sizes[i].size_kb;
        }
    }
    return 0;
}


/*  Returns pointer to catridge type from supplied hex id, returns NULL
 *  if unknown id supplied*/
const char *get_cartridge_type() {
    uint8_t i;
    for (i = 0; i < CARTRIDGE_TYPE_LEN; i++) {
        if(mem[CARTRIDGE_TYPE] == cartridge_types[i].id) {
            return cartridge_types[i].name;
        }
    }
    return NULL;
}  


/*  Returns rom size from supplied hex id, returns 255
 *  if unknown id supplied */
uint8_t get_ram_save_size(uint8_t id) {
    uint8_t i;
    for (i = 0; i < RAM_SAVE_SIZES_LEN; i++) {
        if (id == ram_save_sizes[i].id) {
            return ram_save_sizes[i].size_kb;
        }
    }
    return 255;

}

const char *get_destination_code(uint8_t id) {
    return id ? destination_codes[1] : destination_codes[0];
}

int has_sgb_features() {
    return mem[GB_FUNCTION] == 0x03;
}

int is_colour_compatible() {
    return mem[IS_COLOUR_COMPATIBLE] == 0x80;
}


