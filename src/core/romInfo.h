#ifndef ROM_INFO
#define ROM_INFO

/* Information on game rom currently loaded
 * into memory */

/*--------------ROM defs--------------- */
#define NINTENDO_LOGO_START 0x0104
#define NINTENDO_LOGO_END   0x133

#define ROM_NAME_START 0x0134
#define ROM_NAME_END   0x0142

#define IS_COLOUR_COMPATIBLE 0x143

#define NEW_LICENSE_CODE_MSB 0x0144
#define NEW_LICENSE_CODE_LSB 0x0145

#define GB_FUNCTION 0x0146

#define CARTRIDGE_TYPE 0x0147

#define CARTRIDGE_ROM_SIZE 0x0148
#define CARTRIDGE_RAM_SIZE 0x0149

#define DESTINATION_CODE 0x014A

#define OLD_LICENSE_CODE 0x014B

#define MASK_ROM_VERSION 0x014C

#define COMPLEMENT_CHECKSUM 0x14D

#define CHECKSUM_MSB 0x14E
#define CHECKSUM_LSB 0x14F
/* -------------------------------- */



const char *get_licensee();
uint16_t get_rom_size(uint8_t id);
const char *get_cartridge_type();
uint8_t get_ram_save_size(uint8_t id);
const char *get_destination_code(uint8_t id);
int has_sgb_features();
int is_colour_compatible();


#endif
