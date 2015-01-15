#include "../../non_core/joypad.h"
#include "../../core/mmu/mbc.h"


/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() {
}


/* Check each individual GameBoy key. Returns 1 if
 * the specified key is being held down, 0 otherwise */
int down_pressed()   { return 0; }  
int up_pressed()     { return 0; }
int left_pressed()   { return 0; }
int right_pressed()  { return 0; } 
int a_pressed()      { return 0; }
int b_pressed()      { return 0; }
int start_pressed()  { return 0; }
int select_pressed() { return 0; } 


/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}


/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
void update_keys() {
}









