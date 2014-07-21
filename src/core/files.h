#ifndef FILES_H
#define FILES_H

#define MAX_FILE_SIZE_BYTES 0x20000 //2MB Roms

#define MAX_SRAM_SIZE 0x8000

long load_rom(char *file_path, char *data);

long load_SRAM(char *file_path, char *data);

int save_SRAM(char *file_path, char *data, long size);

#endif //FILES_H
