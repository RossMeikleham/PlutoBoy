#include "../../non_core/files.h"
#include "../../non_core/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#import <Foundation/Foundation.h>

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
    log_message(LOG_INFO, "Loaded file with %d\n bytes\n", count);
    return count;  
}


/* Given a file_path and buffer, attempts to load save data into the buffer
 * up to the suppled size in bytes. Returns the size of the file if successful, 
 * returns 0 if unsuccessful. Buffer should at least be of length size*/
unsigned long load_SRAM(const char *file_path, unsigned char *data, unsigned long size) {
    @autoreleasepool {
        log_message(LOG_INFO, "Attempting to load SRAM for file: %s\n",file_path);
    
        NSURL *documents_dir = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                                   inDomains:NSUserDomainMask] lastObject];
        NSString *end_path = [NSString stringWithCString:file_path];
        NSString *path = [documents_dir.path stringByAppendingPathComponent:end_path];
        
        NSData *load_data  = [NSData dataWithContentsOfFile:path];
        
        if (load_data == nil) {
                log_message(LOG_INFO, "Error opening file: %s\n SRAM not loaded\n", [path cString]);
                return 0;
        }
        
        if ([load_data length] > size) {
            log_message(LOG_INFO, "Bytes read from save is too large!\n");
            return 0;
        }
        
        [load_data getBytes:data length:[load_data length]];
        log_message(LOG_INFO, "Read %lu bytes from SRAM file\n", [load_data length]);
        return [load_data length];
    }
}
 

/* Given a file_path, save data and the size of save data, attempts to
 * save the data to the given file. Returns 1 if successful, 0 otherwise */
int save_SRAM(const char *file_path, const unsigned char *data, unsigned long size) {
    @autoreleasepool {
    
        NSURL *documents_dir = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                                   inDomains:NSUserDomainMask] lastObject];
        NSString *end_path = [NSString stringWithCString:file_path];
        NSString *path = [documents_dir.path stringByAppendingPathComponent:end_path];
        
        NSLog(@"Saving SRAm to Path: %@", path);
        
        NSData *save_data = [NSData dataWithBytesNoCopy:(void*)data length:size freeWhenDone:NO];
        [save_data writeToFile:path atomically:TRUE];
    
    return 1;
  }
}                                      
