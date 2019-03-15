#ifndef UEFI_LIBS
#define UEFI_LIBS

// Custom horribly implemented libC functions, which aren't present/won't link
// in EDK2

#include <stdlib.h>

void *uefi_fopen(const char * const path, const char * const access);

int uefi_fseek(void *stream, long int offset, int origin);

size_t uefi_fread(void *ptr, size_t size, size_t count, void *stream);

void uefi_fclose(void *stream);

char *uefi_strncpy(char *dest, const char * const src, size_t len);

char *uefi_strcat(char *dest, const char * const src);

void *uefi_memmove(void *dst, const void *src, size_t len);

#endif
