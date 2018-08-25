#include "../../non_core/joypad.h"
#include "../../core/mmu/mbc.h"
#include "../../non_core/framerate.h"
#include "../../non_core/logger.h"
#include "debug_info.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t keys_pressed = 0;

void init_joypad() {keys_pressed = 0;}
	
int down_pressed()   { return (keys_pressed & KEY_DDOWN) == KEY_DDOWN; }  
int up_pressed()     { return (keys_pressed & KEY_DUP) == KEY_DUP; }
int left_pressed()   { return (keys_pressed & KEY_DLEFT) == KEY_DLEFT; }
int right_pressed()  { return (keys_pressed & KEY_DRIGHT) == KEY_DRIGHT; } 
int a_pressed()      { return (keys_pressed & KEY_A) == KEY_A; }
int b_pressed()      { return (keys_pressed & KEY_B) == KEY_B; }
int start_pressed()  { return (keys_pressed & KEY_START) == KEY_START; }
int select_pressed() { return (keys_pressed & KEY_SELECT) == KEY_SELECT; }

int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}

int update_keys() {

    if (!aptMainLoop()) {
		write_SRAM();
		return 1;
    }

    hidScanInput();
    
	keys_pressed = hidKeysDown() | hidKeysHeld();

	// Activate frame limiter
	if (keys_pressed & KEY_L) {
		limiter = 1;
	}

	// Deactivate frame limiter
	if (keys_pressed & KEY_R) {
		limiter = 0;
	}

	return 0;
}
