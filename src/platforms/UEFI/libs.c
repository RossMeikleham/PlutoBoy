#include "libs.h"

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellParameters.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellParameters.h>


void *uefi_fopen(const char * const path, const char * const access) {
    
    EFI_FILE_PROTOCOL* root = NULL;
    EFI_FILE_PROTOCOL* file = NULL;

    CHAR16 *wpath = malloc((strlen(path) + 1) * 2);
    if (wpath == NULL) {
        return NULL;
    } 

    for (int i = 0; i < strlen(path) + 1; i++) {
        wpath[i] = (CHAR16)path[i];
    }

    uint64_t write = 0;
    uint64_t read = 0;
    for (int i = 0; i < strlen(access); i++) {
        if (access[i] == 'r') {
            read = EFI_FILE_MODE_READ; 
        }
        if (access[i] == 'w') {
            write = EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE; 
        }
    }
	
    EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_HANDLE* handles = NULL;   
    UINTN handleCount = 0;

    EFI_STATUS efiStatus = gBS->LocateHandleBuffer(ByProtocol, 
                                   &sfspGuid, 
                                   NULL, 
                                   &handleCount, 
                                   &handles);


    for (int index = 0; index < (int)handleCount; index++) {

        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;
        efiStatus = gBS->HandleProtocol(handles[index], &sfspGuid, (void**)&fs);
    
        if(EFI_ERROR(efiStatus)) {
            continue;
        }
    
        efiStatus = fs->OpenVolume(fs, &root);
    
        if (EFI_ERROR(efiStatus)) {
            continue;
        }

        efiStatus = root->Open(
            root, 
            &file,
            wpath,
            write | read,
            0);
    
        if (!EFI_ERROR(efiStatus)) {
            free(wpath);
            return file; 
        }
    }
    
    free(wpath);
    return NULL;
}


// Doesn't support "origin" argument, just sets to offset from start of file
int uefi_fseek(void *stream, long int offset, int origin) {
    EFI_FILE_PROTOCOL* file = (EFI_FILE_PROTOCOL *)stream;
    int res  = file->SetPosition(file, offset);
	return res;
}

size_t uefi_fread(void *ptr, size_t size, size_t count, void *stream) {
    EFI_FILE_PROTOCOL* file = (EFI_FILE_PROTOCOL *)stream;
    UINTN size_read = size * count;
    file->Read(file, &size_read, ptr);
    return size_read / size;
}


void uefi_fclose(void *stream) {
    EFI_FILE_PROTOCOL* file = (EFI_FILE_PROTOCOL *)stream;;
    file->Close(file);
}


char *uefi_strncpy(char *dest, const char * const src, size_t len) {
    const char *ptr = src;
    int i = 0;
    for (; i < len && *ptr != '\0'; i++) {
        dest[i] = ptr[i];
    }
    
    memset(dest + i, '\0', len - i);
    return dest;
}

char *uefi_strcat(char *dest, const char * const src) {
    char *dest_end = dest + strlen(dest);
    int i = -1;
    do {
        i++;
        dest_end[i] = src[i];
    } while (src[i] != '\0');

    return dest;
}

// Copied from https://www.student.cs.uwaterloo.ca/~cs350/common/os161-src-html/doxygen/html/memmove_8c_source.html
void * uefi_memmove(void *dst, const void *src, size_t len) {
    size_t i;

             if ((uintptr_t)dst < (uintptr_t)src) {
                 /*
                  * As author/maintainer of libc, take advantage of the
                  * fact that we know memcpy copies forwards.
                  */
                 return memcpy(dst, src, len);
         }
 
         /*
          * Copy by words in the common case. Look in memcpy.c for more
          * information.
          */
 
         if ((uintptr_t)dst % sizeof(long) == 0 &&
             (uintptr_t)src % sizeof(long) == 0 &&
             len % sizeof(long) == 0) {
 
                 long *d = dst;
                 const long *s = src;
 
                 /*
                  * The reason we copy index i-1 and test i>0 is that
                  * i is unsigned -- so testing i>=0 doesn't work.
                  */
 
                 for (i=len/sizeof(long); i>0; i--) {
                         d[i-1] = s[i-1];
                 }
         }
         else {
                 char *d = dst;
                 const char *s = src;
 
                 for (i=len; i>0; i--) {
                         d[i-1] = s[i-1];
                 }
         }
 
         return dst;
}
