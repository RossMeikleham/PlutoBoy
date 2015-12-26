#include "../../non_core/files.h"
#include "../../non_core/logger.h"
#include <stdio.h>

#include <SDL_rwops.h>

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/
unsigned long load_rom_from_file(const char* filename, unsigned char *data) {
        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        
        if (rw == NULL) {
            log_message(LOG_ERROR, 
                "Error opening file %s: %s\n", filename, SDL_GetError());       
            return 0;
        }

        Sint64 res_size = SDL_RWsize(rw);
        if (res_size > MAX_FILE_SIZE) {
            log_message(LOG_ERROR,
                "Error opening file %s: File is too large to be a Gameboy ROM\n",
                filename);
            return 0;
        }

        if (res_size == 0) {
            log_message(LOG_WARN, "File %s has a size of 0 bytes\n", filename);
            return 0;
        }

        Sint64 nb_read_total = 0, nb_read = 1;
        unsigned char* buf = data;
        while (nb_read_total < res_size && nb_read != 0) {
                nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }

        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
                log_message(LOG_ERROR, 
                    "Bytes expected (%lu) is not equal to bytes read (%lu)\n",
                    res_size, nb_read_total);
                free(data);
                return 0;
        }

        return res_size;
}



/* Given a file_path and buffer, attempts to load save data into the buffer
 * up to the suppled size in bytes. Returns the size of the file if successful, 
 * returns 0 if unsuccessful. Buffer should at least be of length size*/
unsigned long load_SRAM(const char *file_path, unsigned char *data, unsigned long size) {
    return 0;
}
 

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size) {
    return 0; 
}                                      
