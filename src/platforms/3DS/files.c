#include "../../non_core/files.h"
#include "../../non_core/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/

unsigned long load_rom_from_file(const char *file_path, unsigned char *data, size_t data_size) {
 
    FILE *file;  
    /* open file in binary read mode
     * read byte by byte of ROM into memory */

    if(!(file = fopen(file_path,"rb"))) {
        log_message(LOG_ERROR, "Error opening file %s\n", file_path);
        return 0;
    }

    uint32_t count = 0; 
    unsigned char *data_ptr = data;
    int rc = 0;
    
    //Read file contents into buffer
    //while(count < MAX_FILE_SIZE && (rc = fread(data_ptr, 1, READ_SIZE, file))) {
    while(count < data_size && (rc = fread(data_ptr, 1, READ_SIZE, file))) {
        if (rc < 0) {
            log_message(LOG_ERROR, "Failed to read file\n");
            fclose(file);
            return 0;
        }
        data_ptr += rc;
        count += rc;
    }

    fclose(file);
    log_message(LOG_INFO, "Loaded file with %d\n bytes", count); 
    return count;  
}


/* Given a file_path and buffer, attempts to load save data into the buffer
 * up to the suppled size in bytes. Returns the size of the file if successful, 
 * returns 0 if unsuccessful. Buffer should at least be of length size*/
unsigned long load_SRAM(const char *file_path, unsigned char *data, unsigned long size) {
    
    FILE *file;
    log_message(LOG_INFO, "Attempting to load SRAM for file: %s\n",file_path);

    if(!(file = fopen(file_path,"rb"))) {
        log_message(LOG_INFO, "Error opening file: %s\n SRAM not loaded",file_path);
        return 0;
    }
    
    unsigned long count = 0;
    count = fread(data, sizeof (char), size, file);

    if (count == 0) {
        log_message(LOG_WARN, "Empty file %s\n", file_path);
    }

    fclose(file);

    return count;
}

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size) {
    
    FILE *file;
    log_message(LOG_INFO, "Attempting to write SRAM for file: %s\n",file_path);
    
    if(!(file = fopen(file_path, "wb"))) {
        log_message(LOG_ERROR, "Error attempting to open file for writing: %s\n", file_path);
        return 0;  
    }
    
    unsigned long written_count = fwrite(data, sizeof (char), size, file);
    if (written_count == size) {
        fclose(file);
        log_message(LOG_INFO, "%lu bytes successfully written to file\n",size);
        return 0;
    } else {
        log_message(LOG_ERROR, "Only %lu of %lu bytes written\n",written_count, size);
    }
        
    fclose(file);   
    return 1;
}                                      