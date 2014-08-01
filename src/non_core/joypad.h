#ifndef JOYPAD_H
#define JOYPAD_H

/*  Initialize the joypad, should be called before
 *  any other joypad functions */
void init_joypad();

/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
void update_keys();

/* Return state of one of the 8 GameBoy keys
 * 0 for unpressed, 1 for pressed */
int down_pressed();    
int up_pressed();     
int left_pressed();  
int right_pressed(); 
int a_pressed();       
int b_pressed();      
int start_pressed(); 
int select_pressed();  


/* Returns 1 if any of the 8 GameBoy keys 
 * are pressed, 0 otherwise */
int key_pressed();

#endif //JOYPAD_H

