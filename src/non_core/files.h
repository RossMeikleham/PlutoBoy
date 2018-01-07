#ifndef FILES_H
#define FILES_H

#define MAX_FILE_SIZE 0x800000 //8MB Roms
#define MAX_SRAM_SIZE 0x8000 // 128KB SRAM

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/
unsigned long load_rom_from_file(const char *file_path, unsigned char *data);

/* Given a file_path and buffer, attempts to load save data into the buffer.
 * Returns the size of the file if successful, returns 0 if unsuccessful.
 * Buffer should be at minimum of size "MAX_SRAMS_SIZE" */
unsigned long load_SRAM(const char *file_path, unsigned char *data, unsigned long size);


/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size);

#endif //FILES_H
