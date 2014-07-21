#ifndef FILES_H
#define FILES_H

#include <stdlib.h>

#define MAX_FILE_SIZE 0x200000 //2MB Roms

#define MAX_SRAM_SIZE 0x8000

size_t load_rom_from_file(const char *file_path, char *data);

long load_SRAM(const char *file_path, char *data);

int save_SRAM(const char *file_path, const char *data, long size);

#endif //FILES_H
