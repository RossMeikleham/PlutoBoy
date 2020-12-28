#include "../../non_core/files.h"
#include "../../non_core/logger.h"
#include <stdio.h>

#include <SDL_rwops.h>

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/
unsigned long load_rom_from_file(const char *filename, unsigned char *data, unsigned long buf_size) {
        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        
        if (rw == NULL) {
            log_message(LOG_ERROR, 
                "Error opening file %s: %s\n", filename, SDL_GetError());       
            return 0;
        }

        Sint64 res_size = SDL_RWsize(rw);
        if (res_size != buf_size) {
            log_message(LOG_ERROR,
                "Error opening file %s: File size (%lld) doesn't match header size (%lu)\n",
                filename, res_size, buf_size);
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
    
    log_message(LOG_INFO, "Attempting to load SRAM for file: %s\n",file_path);
    
    SDL_RWops *rw = SDL_RWFromFile(file_path, "rb");
    
    if (rw == NULL) {
        log_message(LOG_ERROR, 
            "Error opening file %s: %s\n", file_path, SDL_GetError());       
        return 0;
    }

    Sint64 res_size = SDL_RWsize(rw);
    if (res_size > size) {
        log_message(LOG_ERROR,
            "Error opening file %s: File is too large to be SRAM snapshot\n",
            file_path);
        return 0;
    }

    if (res_size == 0) {
        log_message(LOG_WARN, "File %s has a size of 0 bytes\n", file_path);
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
 

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size) {
    
    log_message(LOG_INFO, "Attempting to save SRAM for file: %s\n",file_path);
    
    SDL_RWops *rw = SDL_RWFromFile(file_path, "wb");
    
    if (rw == NULL) {
        log_message(LOG_ERROR, 
            "Error opening file %s: %s\n", file_path, SDL_GetError());       
        return 0;
    }

    Uint64 nb_write_total = SDL_RWwrite(rw, data, 1, size);
    
    if (nb_write_total == size) {
        SDL_RWclose(rw);
        log_message(LOG_INFO, "%lu bytes successfully written to file\n",size);
        return 0;
    } else {
        log_message(LOG_ERROR, "Only %lu of %lu bytes written\n",nb_write_total, size);
    }

    
    SDL_RWclose(rw);
    return 1; 
}                                      
