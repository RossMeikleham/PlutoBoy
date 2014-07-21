#include "../../non_core/files.h"
#include <stdio.h>

unsigned long load_rom_from_file(const char *file_path, char *data) {
 
    FILE *file;  
    /* open file in binary read mode
     * read byte by byte of ROM into memory */
    if(!(file = fopen(file_path,"rb"))) {
        fprintf(stderr, "Error opening file %s\n", file_path);
        return 0;
    }
 
    unsigned long count = 0; 
    char cur;
    //Read file contents into buffer
    while(count < MAX_FILE_SIZE && fread(&cur, 1, 1, file)) {
        data[count++] = cur;
    }

    if (count == 0) {
       printf(stderr, "Empty file %s\n", file_path);
    }

    fclose(file);
    
    return count;  
}


//TODO
long load_SRAM(const char *file_path, char *data) {
    return 0;
}
 
//TODO
int save_SRAM(const char *file_path, const char *data, long size) {
    return 0;
}
 

                                                                                                             
                                                                                                             
                                      
