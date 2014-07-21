#ifndef FILES_H
#define FILES_H

#define MAX_FILE_SIZE 0x20000 //2MB Roms

#define MAX_SRAM_SIZE 0x8000

long load_rom(const char *file_path, char *data);

long load_SRAM(const char *file_path, char *data);

int save_SRAM(const char *file_path, const char *data, long size);

#endif //FILES_H
