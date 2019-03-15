#include "../../non_core/logger.h"

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
#include <string.h>
#include <stdarg.h>
#include <Guid/FileInfo.h>

#define FILE VOID*

static LogLevel current_log_level = LOG_OFF;
static int init = 0;
static int offset = 0;

static EFI_FILE_PROTOCOL* root = NULL;
static EFI_FILE_PROTOCOL* logfile = NULL;
static CHAR16* logfile_path =  L"logfile.txt";

void set_log_level(LogLevel ll) {
    
    if (ll >= LOG_OFF) {
        current_log_level = LOG_OFF;
    } else {
        current_log_level = ll;
    }
}

static unsigned uint_to_str_padded(unsigned i, char *buf, unsigned max_digits) {
    int digits = 0;
    
    do {
        buf[digits] = (i % 10) + '0';
        i /= 10;
        digits++;
     } while (i != 0);

    while (digits < max_digits) {
        buf[digits] = '0';
        digits++;
    }

     // Digits in string are back-to-front, swap them round
     for (int j = 0; j < digits/2; j++) {
        char temp = buf[j];
        buf[j] = buf[digits - j - 1];
        buf[digits - j - 1] = temp;
     }

     return digits;
}
 
static unsigned uint_to_str(unsigned i, char *buf) {
    int digits = 0;
    
    do {
        buf[digits] = (i % 10) + '0';
        i /= 10;
        digits++;
     } while (i != 0);

     // Digits in string are back-to-front, swap them round
     for (int j = 0; j < digits/2; j++) {
        char temp = buf[j];
        buf[j] = buf[digits - j - 1];
        buf[digits - j - 1] = temp;
     }

     return digits;
}

 
static unsigned uint_to_hex_str(unsigned i, char *buf, int uppercase) {
    int digits = 0;
    
    do {
        int value = i & (0x10 - 1);
        char letter = uppercase ? 'A' : 'a';
        buf[digits] = value < 0xA ? value + '0' : value - 10 + letter;
        i >>= 4;
        digits++;
     } while (i != 0);

     // Digits in string are back-to-front, swap them round
     for (int j = 0; j < digits/2; j++) {
        char temp = buf[j];
        buf[j] = buf[digits - j - 1];
        buf[digits - j - 1] = temp;
     }

     return digits;
}

//Logs the current time to the given stream
static void log_time() {
    
    char buf[100];
    EFI_TIME    Time;

    gRT->GetTime (&Time, NULL);

    char *buf_ptr = buf;
    
    buf_ptr += uint_to_str_padded(Time.Year, buf_ptr, 4);
    *buf_ptr = '-';
    buf_ptr++;
    
    buf_ptr += uint_to_str_padded(Time.Month, buf_ptr, 2);
    *buf_ptr = '-';
    buf_ptr++;

    buf_ptr += uint_to_str_padded(Time.Day, buf_ptr, 2);
    *buf_ptr = ' ';
    buf_ptr++;
    
    buf_ptr += uint_to_str_padded(Time.Hour, buf_ptr, 2);
    *buf_ptr = ':';
    buf_ptr++;
    
    buf_ptr += uint_to_str_padded(Time.Minute, buf_ptr, 2);
    *buf_ptr = ':';
    buf_ptr++;
    
    buf_ptr += uint_to_str_padded(Time.Second, buf_ptr, 2);
    *buf_ptr = ' ';
    buf_ptr++;
    *buf_ptr = '\0';
    
    UINTN bufSize = strlen(buf);
    offset += bufSize;
    logfile->Write(logfile, &bufSize, (CHAR16 *)buf);
}       
/*    
    char buffer[30];
    struct tm *str_time;

    time_t now = time (0);
    str_time = gmtime(&now);

    strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S ", str_time);
    fprintf(stream,"%s ",buffer);
*/
//}

void init_logfile() {
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
            &logfile,
            logfile_path,
            EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ,
            0);
    
        if (EFI_ERROR(efiStatus)) {
            continue;    
        }

        CHAR8 *buf = "LOGFILE\n";
        UINTN bufSize = 8;
        offset += bufSize;

        efiStatus = logfile->Write(logfile, &bufSize, (CHAR16 *)buf);
        if (EFI_ERROR(efiStatus)) {
            continue;    
        }
        
        efiStatus = logfile->Close(logfile);
        if (EFI_ERROR(efiStatus)) {
            continue;
        }

        break;
    }

    init = 1;
}


	
void log_message(LogLevel ll, const char *fmt, ...) {

       
    if(ll <= LOG_OFF && ll >= current_log_level) {
    
        if (!init) {
            init_logfile();
        }
        // Choose output stream based on current log level

        //FILE *stream = (ll == LOG_INFO) ? stdout : stderr;

        // Log message level
        char *level_str = "";
        switch (ll) {
            case LOG_INFO : level_str = "[INFO] "; break;
            case LOG_WARN : level_str = "[WARN] "; break;
            case LOG_ERROR: level_str = "[ERROR] "; break;
            default : level_str = "[UNKNOWN] "; // Shouldn't happen 
        }
        
        EFI_STATUS efiStatus = root->Open(
            root, 
            &logfile,
            logfile_path,
            EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ,
            0);
    
        // Can't do anything, just drop out
        if (EFI_ERROR(efiStatus)) {
            return;    
        }
        
        logfile->SetPosition(logfile, offset);
   
        log_time();
        UINTN level_len = strlen(level_str);
        logfile->Write(logfile, &level_len, (CHAR16 *)level_str); 
        offset += level_len;
    
        // Log message
        va_list args;
        va_start(args, fmt);
    
        //static unsigned uint_to_hex_str(unsigned i, char *buf, int uppercase) {
        // static unsigned uint_to_str(unsigned i, char *buf) {
        char buffer[1024]; // Buffer overflow risk but oh well
        int buffer_index = 0;
        int len = strlen(fmt);
        
		for (int i = 0; i < len; i++) {
            for(; fmt[i] != '%' && i < len; i++) {
                buffer[buffer_index] = fmt[i];
                buffer_index++;
            }

            if (i >= len) {
                break;
            }
            i++;
            
			switch (fmt[i]) {
                case 'd' : {
                    int x = va_arg(args, int);
                    char int_buf[100];
                    unsigned len = uint_to_str((unsigned)x, int_buf);
                    memcpy(buffer + buffer_index, int_buf, len);
                    buffer_index += len;
                    break;
                }
                case 's' : {  
                    char *s = va_arg(args, char*);
                    int len = strlen(s);
                    memcpy(&buffer[buffer_index], s, len);
                    buffer_index += len;
                    break;
               }
				case 'l' : {
					if (i + 1 < len) {
						switch (fmt[i+1]) {
							case 's' : { // "long"/"wide" string
								CHAR16 *s = va_arg(args, CHAR16 *);
								UINTN len = StrLen(s);
								for (int i = 0; i < len; i++) {
									buffer[buffer_index++] = (char)(s[i]);
								}
								i++;
							}
						}
					}
				}
                case 'x' : {
                    unsigned x  = VA_ARG(args, unsigned);
                    char int_buf[100];
                    unsigned len = uint_to_hex_str(x, int_buf, 0);
                    memcpy(&buffer[buffer_index], int_buf, len);
                    buffer_index += len;
                    break;
                }
                case 'X' : {
                    unsigned x  = VA_ARG(args, unsigned);
                    char int_buf[100];
                    unsigned len = uint_to_hex_str(x, int_buf, 1);
                    memcpy(&buffer[buffer_index], int_buf, len);
                    buffer_index += len;
                    break;
                }
                default:
                    break;
            }
        }

        va_end(args);        
        
        offset += buffer_index;
    
        efiStatus = logfile->Write(logfile, (UINTN *)(&buffer_index), (CHAR16 *)buffer);
        logfile->Close(logfile); 
    } 
}
