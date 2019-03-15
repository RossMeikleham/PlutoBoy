#include "../../non_core/joypad.h"

#include "../../core/mmu/mbc.h"
#include "../../non_core/logger.h"

#include <stdlib.h>

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/SimpleTextIn.h>

typedef struct {
	int scan_code;
    int key_code; // Key code on the keyboard which
                       // maps to the given gameboy button
    int state; // 1 pressed, 0 unpressed
} button_state;

enum {UP = 0, DOWN, LEFT, RIGHT, A, B, START, SELECT};

button_state buttons[8];

#define TOTAL_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))

UINT16 button_config_scan_codes[] = {SCAN_UP, SCAN_DOWN, SCAN_LEFT, SCAN_RIGHT, 0, 0, 0 ,0};
CHAR16 button_config_key_codes[] = {0, 0, 0, 0, L'a', L's', CHAR_CARRIAGE_RETURN, L' '};

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() { 
     
    buttons[UP].state = 0;
    buttons[UP].key_code = button_config_key_codes[UP];
    buttons[UP].scan_code = button_config_scan_codes[UP];

    buttons[DOWN].state = 0;
    buttons[DOWN].key_code = button_config_key_codes[DOWN];
    buttons[DOWN].scan_code = button_config_scan_codes[DOWN];

    buttons[LEFT].state = 0;
    buttons[LEFT].key_code = button_config_key_codes[LEFT];
    buttons[LEFT].scan_code = button_config_scan_codes[LEFT];

    buttons[RIGHT].state = 0;
    buttons[RIGHT].key_code = button_config_key_codes[RIGHT];
    buttons[RIGHT].scan_code = button_config_scan_codes[RIGHT];

    buttons[A].state = 0;
    buttons[A].key_code = button_config_key_codes[A];
    buttons[A].scan_code = button_config_scan_codes[A];

    buttons[B].state = 0;
    buttons[B].key_code = button_config_key_codes[B];
    buttons[B].scan_code = button_config_scan_codes[B];

    buttons[START].state = 0;
    buttons[START].key_code = button_config_key_codes[START];
    buttons[START].scan_code = button_config_scan_codes[START];
   
    buttons[SELECT].state = 0;
    buttons[SELECT].key_code = button_config_key_codes[SELECT];
    buttons[SELECT].scan_code = button_config_scan_codes[SELECT];
}

/* Check each individual GameBoy key. Returns 1 if
 * the specified key is being held down, 0 otherwise */
int down_pressed()   { return buttons[DOWN].state;  }  
int up_pressed()     { return buttons[UP].state; }
int left_pressed()   { return buttons[LEFT].state;}
int right_pressed()  { return buttons[RIGHT].state;} 
int a_pressed()      { return buttons[A].state; }
int b_pressed()      { return buttons[B].state;}
int start_pressed()  { return buttons[START].state; }
int select_pressed() { return buttons[SELECT].state; } 

/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}

void unset_keys() {
    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        buttons[i].state = 0;
    }
}

/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
int update_keys() {
                    
	for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
		buttons[i].state = 0;
	}

	// Get all pending input events
	EFI_STATUS status = EFI_SUCCESS;
	EFI_INPUT_KEY inputKey;

	while (1)
	{
		status = gST->ConIn->ReadKeyStroke(gST->ConIn, &inputKey);
		if (status == EFI_NOT_READY)
		{
			// No more key strokes
			break;
		}
		else if (EFI_ERROR(status))
		{
			break;
		}
	
		for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        	if (buttons[i].scan_code == inputKey.ScanCode) {
				if (inputKey.ScanCode != 0) {
                	buttons[i].state = 1;
					break;
				}
				if (buttons[i].key_code == inputKey.UnicodeChar) {
					buttons[i].state = 1;
					break;
				}
			}
		}
	}

	return 0;
}
