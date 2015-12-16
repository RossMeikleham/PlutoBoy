#include "mmu/memory.h"
#include "rom_info.h"

#include <stdint.h>

/*--------  Cartridge Header info---- */

typedef struct {
    uint8_t id;
    char *name;
} licensee;


typedef struct {
    uint8_t id;
    char *name;
} cartridge_type;


typedef struct {
    uint8_t id;
    uint16_t size_kb;
} rom_size;


typedef struct {
    uint8_t id;
    uint8_t size_kb;
} ram_save_size;

 /*  ----------------------------------- */


#define LICENSES_LEN sizeof(licenses)/sizeof(licensee)
#define OLD_LICENSES_LEN sizeof(old_licenses)/sizeof(licensee)
#define ROM_SIZES_LEN sizeof(rom_sizes)/sizeof(rom_size)
#define CARTRIDGE_TYPE_LEN sizeof(cartridge_types)/sizeof(cartridge_type)  
#define RAM_SAVE_SIZES_LEN sizeof(ram_save_sizes)/sizeof(ram_save_size)


static licensee licenses[] = {{0, "none"}, {1, "nintendo"}, {8, "capcom"}, {13, "electronic arts"}, {18, "hudsonsoft"}, {19, "b-ai"},
     {20, "kss"}, {22, "pow"}, {24, "pcm complete"}, {25, "san-x"}, {28, "kemco japan"}, {29, "seta"},
     {30, "viacom"}, {31, "nintendo"}, {32, "bandai"}, {33, "ocean/acclaim"}, {34, "konami"}, {35, "hector"}, {37, "taito"},
     {38, "hudson"}, {39, "banpresto"}, {41, "ubi soft"}, {42, "atlus"}, {44, "malibu"}, {46, "angel"}, 
     {47, "pullet-proof"}, {49, "irem"}, {50, "absolute"}, {51, "acclaim"}, {52, "activision"},
     {53, "american sammy"}, {54, "gametek"}, {55, "park place"}, {56, "ljn"}}; 

static licensee old_licenses[] = {{0, "none"}, {1, "nintendo"}, {8, "capcom"}, {13, "electronic arts"}, {18, "hudsonsoft"}, {19, "itc entertainment"},
      {24, "pcm complete"}, {25, "san-x"}, {28, "kotobuki systems"}, {29, "seta"},
     {30, "infogames"}, {31, "nintendo"}, {32, "bandai"}, {34, "konami"}, {35, "hector"}, 
     {38, "capcom"}, {39, "banpresto"}, {41, "ubi soft"}, {42, "atlus"}, {44, "malibu"}, {46, "angel"}, 
     {47, "spectrum holoby"}, {50, "absolute"}, {51, "acclaim"}, {52, "activision"},
     {53, "american sammy"}, {54, "konami"}, {55, "hi tech entertainment"}, {56, "ljn"}}; 


static rom_size rom_sizes[] = {{0x0, 32}, {0x1, 64}, {0x2, 128}, {0x3, 256}, {0x4, 512},
    {0x5, 1024}, {0x6, 2048}, {0x7, 4096}, {0x8, 8192}};

static cartridge_type cartridge_types[] = {{0x00 ,"ROM"}, {0x01, "ROM+MBC1"},
    {0x02, "ROM+MBC1+RAM"}, {0x03, "ROM+MBC1+RAM+BATTERY"}, {0x05, "ROM+MBC2"}, 
    {0x06, "ROM+MBC2+RAM+BATTERY"}, {0x08, "ROM+RAM"},
    {0x09, "ROM+RAM+BATTERY"}, {0x0B, "ROM+MMM01"}, {0x0C, "ROM+MMM01+RAM"}, {0x0D, "ROM+MMM01+RAM+BATTERY"},
    {0x0F, "ROM+MBC3+TIMER+BATTERY"}, {0x10, "ROM+MBC3+TIMER+RAM+BATTERY"}, {0x11, "ROM+MBC3"},
    {0x12, "ROM+MBC3+RAM"}, {0x13, "ROM+MBC3+RAM+BATTERY"}, {0x19, "ROM+MBC5"}, {0x1A, "ROM+MBC5+RAM"}, 
    {0x1B, "ROM+MBC5+RAM+BATTERY"},
    {0x1C, "ROM+MBC5+RUMBLE"}, {0x1D, "ROM+MBC5+RUMBLE+RAM"}, {0x1E, "ROM+MBC5+RUMBLE+RAM+BATTERY"}, 
    {0x20, "ROM+MBC6+RAM+BATTERY"}, {0x22, "ROM+MBC7+RAM+BATTERY+ACCELEROMETER"}, 
    {0xFC, "POCKET CAMERA"}, {0xFD, "BANDAI TAMA5"}, {0xFE, "HuC3+RAM+BATTERY+RTC"}, {0xFF, "HuC1+RAM+BATTERY"}};



static ram_save_size ram_save_sizes[] = {{0, 0}, {1, 2}, {2, 8}, {3, 32}, {5, 64}};

static char *destination_codes[] = {"Japanese", "Non-Japanese"};




/*  Returns pointer to liscensee,
 *  Returns NULL if unknown id in loaded rom */
const char *get_licensee() {

   uint8_t i, id;

   /*  IF old licence code location contains 0x33
    *  the licence code is stored as 0xXY where 3X
    *  is the MSB of the new license code location
    *  and 3Y is the LSB of the new licence code location*/
   if (get_mem(OLD_LICENSE_CODE) == 0x33) {
       id = get_mem(NEW_LICENSE_CODE_MSB) << 0x4 | (
           get_mem(NEW_LICENSE_CODE_LSB) & 0xF);
       for (i = 0; i < LICENSES_LEN; i++) {
           if(id == licenses[i].id) {
               return licenses[i].name;
           }
       }
   /* Otherwise licence code is stored at old licence location */
   } else {
       id = get_mem(OLD_LICENSE_CODE);
       for (i = 0; i < OLD_LICENSES_LEN; i++) {
           if (id == old_licenses[i].id) {
               return old_licenses[i].name;
           }
       }
   }
   return NULL;
   
}


/*  Returns ROM size in KB from id, returns 0
 *  if unknown id currently in memory */
uint16_t id_to_rom_size(uint8_t id) {
    
    for (unsigned  i = 0; i < ROM_SIZES_LEN; i++) {
        if (rom_sizes[i].id == id) {
            return rom_sizes[id].size_kb;
        }
    }
    return 0;
}

/*  Returns ROM size in KB, returns 0
 *  if unknown id currently in memory */
uint16_t get_rom_size() {
    
    uint8_t id = get_mem(CARTRIDGE_ROM_SIZE);
    return id_to_rom_size(id);
}



/*  Returns pointer to catridge type, returns NULL
 *  if unknown id in memory*/
const char *get_cartridge_type() {

    uint8_t i;
    for (i = 0; i < CARTRIDGE_TYPE_LEN; i++) {
        if(get_mem(CARTRIDGE_TYPE) == cartridge_types[i].id) {
            return cartridge_types[i].name;
        }
    }
    return NULL;
}  


/*  Returns ram save size in KB, returns 255
 *  if unknown id currently in memory */
uint8_t id_to_ram_save_size(uint8_t id) {
    for (unsigned  i = 0; i < CARTRIDGE_RAM_SIZE; i++) {
        if (ram_save_sizes[i].id == id) {
            return ram_save_sizes[id].size_kb;
        }
    }
    return 255;
}


/*  Returns ram save size in KB, returns 255
 *  if unknown id currently in memory */
uint8_t get_ram_save_size() {
   return id_to_ram_save_size(get_mem(CARTRIDGE_RAM_SIZE));
}


/* Returns pointer to destination string of the currently loaded ROM
 * Either Japanese or Non-Japanese */
const char *get_destination_code() {

    return get_mem(DESTINATION_CODE) ? destination_codes[1] : destination_codes[0];
}

/* Returns 1 if loaded ROM has Super Gameboy Specific features,
 * 0 otherwise */
int has_sgb_features() {

    return get_mem(GB_FUNCTION) == 0x03;
}

/* Returns 1 if loaded ROM has Gameboy Color specific features,
 * 0 otherwise */  
int is_colour_compatible() {

    return get_mem(IS_COLOUR_COMPATIBLE) == 0x80;
}


int is_colour_only() {
    return get_mem(IS_COLOUR_COMPATIBLE) == 0xC0;
}



