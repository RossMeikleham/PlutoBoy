#include <SDL.h>

#include "../../non_core/joypad.h"
#include "../../core/mmu/mbc.h"

#define KEY_NO 14
static char keys[KEY_NO];

typedef enum {TRIANGLE, CIRCLE, CROSS, SQUARE, LTRIGGER, RTRIGGER,
             DOWN, LEFT, UP, RIGHT, SELECT, START, HOME, CTRL_HOLD} Button;

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() {
    
    for(int i = 0; i < KEY_NO; i++) {
        keys[i] = 0;
    }

    SDL_EnableKeyRepeat(0,0);
}


/* Check each individual GameBoy key. Returns 1 if
 * the specified key is being held down, 0 otherwise */
int down_pressed()   { return keys[DOWN]; }  
int up_pressed()     { return keys[UP]; }
int left_pressed()   { return keys[LEFT]; }
int right_pressed()  { return keys[RIGHT]; } 
int a_pressed()      { return keys[CROSS]; }
int b_pressed()      { return keys[CIRCLE]; }
int start_pressed()  { return keys[START]; }
int select_pressed() { return keys[SELECT]; } 


/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}


/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
void update_keys() {
    
    SDL_Event e;
    
    if (SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT:
                exit(1);
            case SDL_JOYBUTTONDOWN:
                keys[e.jbutton.button] = 1; 
                break;
            case SDL_JOYBUTTONUP:
                keys[e.jbutton.button] = 0;
                break;
                
        }

    }
}









