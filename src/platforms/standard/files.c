#include "../../NonCoreHeaders/files.h"
#include <stdio.h>

long load_rom(const char *file_path, char *data) {
 
    FILE *file;  
    /* open file in binary read mode
     * read byte by byte of ROM into memory */
    if(!(file = fopen(file_path,"rb"))) {
        fprintf(stderr, "Error opening file %s\n", file_path);
        return -1;
    }
 
    long count; 
    for (count = 0; count < MAX_FILE_SIZE; count++) {
    //Read file contents into buffer
    char cur;
        while(fread(&cur, 1, 1, file)) {
            data[count] = cur;
        }
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
 

                                                                                                             
                                                                                                             
                                      
