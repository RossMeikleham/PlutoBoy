#ifndef FILES_H
#define FILES_H

#include <stdlib.h>

#define MAX_FILE_SIZE 0x800000 //8MB Roms
#define MAX_SRAM_SIZE 0x8000 // 128KB SRAM

#define READ_SIZE 4096

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/
unsigned long load_rom_from_file(const char *file_path, unsigned char *data, size_t data_size);

/* Given a file_path and buffer, attempts to load save data into the buffer.
 * Returns the size of the file if successful, returns 0 if unsuccessful.
 * Buffer should be at minimum of size "MAX_SRAMS_SIZE" */
unsigned long load_SRAM(const char *file_path, unsigned char *data, unsigned long size);

/* Given a memory location, buffer, and size of the cache, attempts to load save data from cache into the buffer.
 * Returns the size of the cache if successful, returns 0 if unsuccessful.
 * Buffer should be at minimum of size "MAX_SRAMS_SIZE" */
unsigned long load_SRAM_cached(unsigned char *cache_ptr, unsigned char *data, unsigned long size);

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size);

/* Given memory location, save data and the size of save data, attempts to
 * save the data to a cache in ram. Used to improve 3DS emulation speed 
 * and lower the amount of taxing I/O writes. Returns 1 if successful, 0 otherwise */
int save_SRAM_cached(unsigned char *cache_ptr, unsigned char *data, unsigned long size);

#endif //FILES_H
