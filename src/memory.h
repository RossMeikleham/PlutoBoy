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



/* -------------IO ports/registers ------------------*/
#define P1_REG 0xFF00 /* Register for reading joy pad info */
#define SB_REG 0xFF01 /* Serial transfer data */
#define SC_REG 0xFF02 /* SIO control */
#define DIV_REG 0xFF04 /* Divider register*/
#define TIMA_REG 0xFF05 /* Timer Counter */
#define TMA_REG 0xFF06 /* Timer Modulo  */
#define TAC_REG 0xFF07 /* Timer Control */
#define IF_FLAG 0xFF0F /* Interrupt Flag */

/*  Sound Mode 1 registers */
#define NR_10_REG 0xFF10 /* Sweep */
#define NR_11_REG 0xFF11 /* Length wave pattern*/
#define NR_12_REG 0xFF12 /* Envelope */
#define NR_13_REG 0xFF12 /* Frequency Lo */
#define NR_14_REG 0xFF14 /* Frequency Hi */

/*  Sound Mode 2 registers */
#define NR_21_REG 0xFF16 /* Length wave pattern*/
#define NR_22_REG 0xFF17 /* Envelope */
#define NR_23_REG 0xFF18 /* Frequency Lo */
#define NR_24_REG 0xFF19 /* Frequency Hi */

/*  Sound Mode 3 registers */
#define NR_30_REG 0xFF1A /* Sound on/off*/
#define NR_31_REG 0xFF1B /* Sound length */
#define NR_32_REG 0xFF1C /* Select output level */
#define NR_33_REG 0xFF1D /* Frequency Lo */
#define NR_34_REG 0xFF1E /* Frequency Hi */

/*  Sound Mode 4 registers */
#define NR_41_REG 0xFF20 /* Sound length */
#define NR_42_REG 0xFF21 /* Envelope */
#define NR_43_REG 0xFF22 /* Polynomial Counter */
#define NR_44_REG 0xFF23 /* Counter/Consecutive; initial */

#define NR_50_REG 0xFF24 /* Channel Control/on-off/Volume */
#define NR_51_REG 0xFF25 /* Selection of Sound output terminal */
#define NR_52_REG 0xFF26 /* Sound on/off */

/* Waveform Storage for Arbitrary Sound data */
#define WAVE_PATTERN_RAM_START 0xFF30
#define WAVE_PATTERN_RAM_END   0xFF3F

/*------Screen/Graphics register locations------- */
#define LCDC_REG 0xFF40 /* LCD Control */
#define STAT_REG 0xFF41 /* LCD status */
#define SCROLL_Y_REG 0xFF42 /* 8 bit value to scroll BG Y pos */
#define SCROLL_X_REG 0xFF43 /* 8 bit value to scroll BG X pos */
#define LY_REG 0xFF44 /* LCDC Y-Coordinate */
#define LYC_REG 0xFF45 /* LY Compare */
#define DMA_REG 0xFF46 /* DMA Transfer and Start Address */
#define BGP_REF 0xFF47 /* BG and Window Palette Data */
#define OBP0_REG 0xFF48 /* Object Palette 0 Data */
#define OBP1_REG 0xFF49 /* Object Palette 1 Data */
#define WY_REG 0xFF4A /* Window Y Position; 0 <= WY <= 143*/
#define WX_REG 0xFF4B /* Window X Position; 0 <= WX <= 166 */

#define IE_REG 0xFFFF /* Interrupt Enable Register */



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
