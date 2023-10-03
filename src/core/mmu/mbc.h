#ifndef MBC_H
#define MBC_H

#include <stdint.h>

#define RAM_BANK_SIZE 0x2000 // 8KB
#define ROM_BANK_SIZE 0x4000 // 16KB

// used to delay the SRAM_write() to file 
// write to the SRAM file only if it's been XX seconds since we last wrote to it
#define SRAM_WRITE_DELAY 60000 // 60 seconds

extern uint8_t *RAM_banks;//[][0x2000];  // max 16 * 8KB ram banks (128KB) 0x2000
extern uint8_t *ROM_banks;//[][0x4000];// max 512 * 16KB rom banks (8MB) 0x4000

extern unsigned ROM_bank_count;
extern unsigned RAM_bank_count;

typedef enum {SRAM = 0x1, BATTERY = 0x2, RTC = 0x4, RUMBLE = 0x8, ACCELEROMETER = 0x10} features;

// Real time clock registers for MBC3
typedef struct {
	uint8_t seconds; //0 - 59
	uint8_t minutes; //0 - 59
	uint8_t hours; //0 - 23
	uint8_t days_low; // lower 8 bit of days
	uint8_t flags; // upper bit of days + overflow and halt bits
				   // bit 0 contains 9th bit of days, 
				   // bit 6 contains halt flag
				   // bit 7 day carry overflow
} rtc_regs_MBC3;



/*  Setup a memory bank controller for the given
 *  cartridge type id. Returns 1 if successful,
 *  0 if not implemented or invalid. */
int setup_MBC(int no, unsigned ram_banks, unsigned rom_banks, char const *file_name);


// Frees RAM/ROM banks
void teardown_MBC();


/* Writes/Reads ROM SRAM from file, used for
 * save games */
void write_SRAM();
void flush_SRAM();	// force write to file
void read_SRAM();


//Increments the RTC clock in MBC3
void inc_sec_mbc3();

/* Writes/Reads RTC data from file,
   used to emulate MBC3 cartridge clock battery
   while the game is turned off */
rtc_regs_MBC3 read_RTC();
void write_RTC(rtc_regs_MBC3 rtc_regs);


/*  Placeholders for write/read function ptrs
 *  depending on MBC mode */
typedef uint8_t (*read_MBC_ptr)(uint16_t addr);
typedef void   (*write_MBC_ptr)(uint16_t addr, uint8_t val);

extern read_MBC_ptr read_MBC;
extern write_MBC_ptr write_MBC; 


#endif //MBC_H
