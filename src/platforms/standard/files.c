#include "../../non_core/files.h"
#include <stdio.h>

/*  Given a file_path and buffer to store file data in, attempts to
 *  read the file into the buffer. Returns the size of the file if successful,
 *  returns 0 if unsuccessful. Buffer should be at minimum of size "MAX_FILE_SIZE"*/
unsigned long load_rom_from_file(const char *file_path, unsigned char *data) {
 
    FILE *file;  
    /* open file in binary read mode
     * read byte by byte of ROM into memory */
    if(!(file = fopen(file_path,"rb"))) {
        fprintf(stderr, "Error opening file %s\n", file_path);
        return 0;
    }
 
    unsigned long count = 0; 
    unsigned char cur;
    //Read file contents into buffer
    while(count < MAX_FILE_SIZE && fread(&cur, 1, 1, file)) {
        data[count++] = cur;
    }

    if (count == 0) {
       fprintf(stderr, "Empty file %s\n", file_path);
    }

    fclose(file);
    
    return count;  
}


/* Given a file_path and buffer, attempts to load save data into the buffer.
 * Returns the size of the file if successful, returns 0 if unsuccessful.
 * Buffer should be at minimum of size "MAX_SRAMS_SIZE" */
long load_SRAM(const char *file_path, unsigned char *data) {
    //TODO
    return 0;
}
 

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, long size) {
    //TODO
    return 0;
}                                      
