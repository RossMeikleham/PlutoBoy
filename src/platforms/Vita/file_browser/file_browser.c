#include "file_browser.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/sysmem.h>

#include "../debugScreen.h"
#include "../../../non_core/logger.h"

#define printf psvDebugScreenPrintf

#define MAX_FILES 256
#define MAX_PATH_LENGTH 4096

typedef struct {
	int is_dir;
	char name[255];
	char path[MAX_PATH_LENGTH];
	int size;
} File;

File files[MAX_FILES]; 
int file_count = 0;
int current_file = 0;

void folder_scan(const char *path) {

	SceIoDirent g_dir;
	file_count = 0;
	int fd = sceIoDopen(path);

    if (fd) {
	
		// ".."	
		files[0].is_dir = 1;
		sprintf(files[0].name, "..");

		// Obtain the path for the parent directory
		const char *last_dir_chr = strrchr(path, '/');
		if (last_dir_chr != NULL) {
			strncpy(files[0].path, path, last_dir_chr - path);
		// Already as far up as we can go
		} else {
			strncpy(files[0].path, path, MAX_PATH_LENGTH);
		}

		file_count = 1;

        for (; sceIoDread(fd, &g_dir) && file_count < MAX_FILES; file_count++) {
            sprintf(files[file_count].name, g_dir.d_name);
			// TODO bounds check, this can easily overwrite the buffer
            sprintf(files[file_count].path, "%s/%s", path, files[file_count].name);

			// Check if it is a file or directory
            files[file_count].is_dir = SCE_S_ISDIR(g_dir.d_stat.st_mode); 
            files[file_count].size = g_dir.d_stat.st_size;
		}
    }

    sceIoDclose(fd);
}

void print_dir() {

	psvDebugScreenClear(COLOR_DEFAULT_BG);
	for (int i = 0; i < file_count; i++) {

		int font_color = COLOR_WHITE;
		if (i == current_file) {
			font_color = COLOR_GREEN;
		} else if (files[i].is_dir) {
			font_color = COLOR_CYAN;
		}

		psvDebugScreenSetFgColor(font_color);
		printf("%-80s\t%s\n", files[i].name, files[i].is_dir ? "DIR" : "FILE");
	}
} 

char *dir_browse(const char *path) {
    
    psvDebugScreenInit();
	
	int selected_file = 0;
	folder_scan(path);
	print_dir();

	SceCtrlData ctrl;	
	while (!selected_file) {
		do {
			sceCtrlPeekBufferPositive(0, &ctrl, 1);
    		sceKernelDelayThread(100000); // Wait for 0.1 seconds
		} while ((ctrl.buttons != SCE_CTRL_CROSS)
			&& (ctrl.buttons != SCE_CTRL_UP)
			&& (ctrl.buttons != SCE_CTRL_DOWN));

		// Previous file
		if ((ctrl.buttons == SCE_CTRL_UP) && current_file > 0) {
			current_file--;
		// Next file
		} else if ((ctrl.buttons == SCE_CTRL_DOWN) && current_file < (file_count - 1)) {
			current_file++;
		// Select File/Dir
		} else if (ctrl.buttons == SCE_CTRL_CROSS) {
			if (!files[current_file].is_dir) {
				selected_file = 1;
			} else {
				char new_path[MAX_PATH_LENGTH];
				strncpy(new_path, files[current_file].path, MAX_PATH_LENGTH);
				folder_scan(new_path);
				current_file = 0;
			}
		}

		if (!selected_file) {
			print_dir();
		}
	}
	
	return files[current_file].path;
}
