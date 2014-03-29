#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>

#define WORD_MAX 0xFFFF
#define BYTE_MAX 0xFF

/* Memory definitions */

#define INTERRUPT_VECTORS_START 0x0000
#define INTERRUPT_VECTORS_END   0x00FF

#define CART_HEADER_START 0x0100
#define CART_HEADER_END 0x014F

#define CART_ROM_BANK0_START 0x0150
#define CART_ROM_BANK0_END   0x3FFF

#define CART_ROM_SWITCH_BANK_START 0x4000
#define CART_ROM_SWITCH_BANK_END   0x7FFF
  
#define TILE_SET_0_START 0x8000
#define TILE_SET_0_END   0x8FFF

#define TILE_SET_1_START 0x8800
#define TILE_SET_1_END   0x97FF

#define BG_MAP_DATA1_START 0x9800
#define BG_MAP_DATA1_END 0x9BFF

#define BG_MAP_DATA2_START 0x9C00
#define BG_MAP_DATA2_END   0x9FFF

#define CART_RAM_START 0xA000
#define CART_RAM_END   0xBFFF

#define INTERNAL_RAM_BANK0_START 0xC000
#define INTERNAL_RAM_BANK0_END   0xCFFF

#define INTERNAL_RAM_SWITCH_BANK_START 0xD000
#define INTERNAL_RAM_SWITCH_BANK_END   0xDFFF

#define ECHO_RAM_START 0xE000 /*  Mirrored Memory */
#define ECHO_RAM_END   0xFDFF 

/*--------------- HIGH MEM-------------- */
#define SPRITE_ATTRIBUTE_TABLE_START 0xFE00
#define SPRITE_ATTRIBUTE_TABLE_END   0xFE9F

#define UNUSABLE_MEM_START 0xFEA0
#define UNUSABLE_MEM_END 0xFEFF

#define IO_PORTS_START 0xFF00
#define IO_PORTS_END 0xFF7F

#define ZERO_PAGE_START 0xFF80
#define ZERO_PAGE_END 0xFFFE

#define INTERRUPT_ENABLE_REG 0xFFFF
/* -------------------------------------*/ 






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

void setMem(uint16_t loc, uint8_t val);
uint8_t getMem(uint16_t loc);

const char *get_licensee();
uint16_t get_rom_size(uint8_t id);
const char *get_cartridge_type();
uint8_t get_ram_save_size(uint8_t id);
const char *get_destination_code(uint8_t id);
int has_sgb_features();
int is_colour_compatible();


#endif
