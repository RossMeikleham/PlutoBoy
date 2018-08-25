#ifndef MEM_LAYOUT
#define MEM_LAYOUT

// Global addresses

#define WORD_MAX 0xFFFF
#define BYTE_MAX 0xFF

//Interrupt service routine addresses
#define VBLANK_ISR_ADDR 0x40
#define LCDC_ISR_ADDR 0x48
#define TIMER_ISR_ADDR 0x50
#define IO_ISR_ADDR 0x58
#define HIGH_LOW_ISR_ADDR 0x60

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

#define BG_MAP_DATA0_START 0x9800
#define BG_MAP_DATA0_END 0x9BFF

#define BG_MAP_DATA1_START 0x9C00
#define BG_MAP_DATA1_END   0x9FFF

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


/*  -------------IO ports/registers ------------------*/
//Convert between local IO memory address and global address
#define IO_TO_GLOBAL_ADDR(A) (A | 0xFF00)

#define UNUSABLE_MEM_START 0xFEA0
#define UNUSABLE_MEM_END 0xFEFF

#define IO_PORTS_START 0x00
#define IO_PORTS_END 0x7F

#define ZERO_PAGE_START 0x80
#define ZERO_PAGE_END 0xFE

#define INTERRUPT_ENABLE_REG 0xFF

#define P1_REG 0x00 /*  Register for reading joy pad info */
#define SB_REG 0x01 /*  Serial transfer data */
#define SC_REG 0x02 /*  SIO control */
#define DIV_REG 0x04 /*  Divider register*/
#define TIMA_REG 0x05 /*  Timer Counter */
#define TMA_REG 0x06 /*  Timer Modulo  */
#define TAC_REG 0x07 /*  Timer Control */
#define INTERRUPT_REG 0x0F /*  Interrupt Flag */

/*   Sound Mode 1 registers */
#define NR_10_REG 0x10 /*  Sweep */
#define NR_11_REG 0x11 /*  Length wave pattern*/
#define NR_12_REG 0x12 /*  Envelope */
#define NR_13_REG 0x12 /*  Frequency Lo */
#define NR_14_REG 0x14 /*  Frequency Hi */

/*   Sound Mode 2 registers */
#define NR_21_REG 0x16 /*  Length wave pattern*/
#define NR_22_REG 0x17 /*  Envelope */
#define NR_23_REG 0x18 /*  Frequency Lo */
#define NR_24_REG 0x19 /*  Frequency Hi */

/*   Sound Mode 3 registers */
#define NR_30_REG 0x1A /*  Sound on/off*/
#define NR_31_REG 0x1B /*  Sound length */
#define NR_32_REG 0x1C /*  Select output level */
#define NR_33_REG 0x1D /*  Frequency Lo */
#define NR_34_REG 0x1E /*  Frequency Hi */

/*   Sound Mode 4 registers */
#define NR_41_REG 0x20 /*  Sound length */
#define NR_42_REG 0x21 /*  Envelope */
#define NR_43_REG 0x22 /*  Polynomial Counter */
#define NR_44_REG 0x23 /*  Counter/Consecutive; initial */

#define NR_50_REG 0x24 /*  Channel Control/on-off/Volume */
#define NR_51_REG 0x25 /*  Selection of Sound output terminal */
#define NR_52_REG 0x26 /*  Sound on/off */

/*  Waveform Storage for Arbitrary Sound data */
#define WAVE_PATTERN_RAM_START 0x30
#define WAVE_PATTERN_RAM_END   0x3F

/* ------Screen/Graphics register locations------- */
#define LCDC_REG 0x40 /*  LCD Control */
#define STAT_REG 0x41 /*  LCD status */
#define SCROLL_Y_REG 0x42 /*  8 bit value to scroll BG Y pos */
#define SCROLL_X_REG 0x43 /*  8 bit value to scroll BG X pos */
#define LY_REG 0x44 /*  LCDC Y-Coordinate */
#define LYC_REG 0x45 /*  LY Compare */
#define DMA_REG 0x46 /*  DMA Transfer and Start Address */
#define BGP_REF 0x47 /*  BG and Window Palette Data */
#define OBP0_REG 0x48 /*  Object Palette 0 Data */
#define OBP1_REG 0x49 /*  Object Palette 1 Data */
#define BOOT_ROM_DISABLE 0x50 /* If boot rom executing, remaps the first 0x100 cartidge bytes back in*/
#define WY_REG 0x4A /*  Window Y Position; 0 <= WY <= 143*/
#define WX_REG 0x4B /*  Window X Position; 0 <= WX <= 166 */

/* DMA tranfer for Gameboy Color */
#define HDMA1_REG 0x51
#define HDMA2_REG 0x52
#define HDMA3_REG 0x53
#define HDMA4_REG 0x54
#define HDMA5_REG 0x55

#define KEY1_REG 0x4D /* Prepare Speed switch for Gameboy Color, used to switch clock speed */

#define VBANK_REG 0x4F /* Select which VRAM bank to use in Gameboy color */

#define INFRARED_REG 0x56 /* Infrared Communications Port for Gameboy Color */

#define BGPI 0x68 // Background Palette index for Gameboy Color
#define BGPD 0x69 // Background Palette data for Gameboy Color
#define SPPI 0x6A // Sprite Palette index for Gameboy Color
#define SPPD 0x6B // Sprite Palette data for Gameboy Color

#define SRAM_BANK 0x70 // Register to select internal RAM banks for Gameboy Color

#define INTERRUPT_ENABLE_REG 0xFF /*  Interrupt Enable Register */

/* ************************************************** */

#endif //MEMORY_LAYOUT_H
