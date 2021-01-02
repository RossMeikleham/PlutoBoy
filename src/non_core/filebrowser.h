#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <stdint.h>

#define READ_SIZE 4096

typedef struct {
    char *name;
    int is_dir;
} dir_entry_t;

typedef struct {
    char *path;
    uint32_t entry_count;
    dir_entry_t *entries;
} dir_t;

dir_t *get_dir(const char * const dir_path);

void free_dir(dir_t * const dir);

#ifdef __ANDROID__
uint8_t *load_asset(const char * const path, int *length);
#endif

#endif // FILE_BROWSER_H
