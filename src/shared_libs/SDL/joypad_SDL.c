#include "SDL/SDL.h"
#include "stdlib.h"
#include "../../non_core/joypad.h"

static int keys[322];  // 322 SDL keys


/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() {

    for(int i = 0; i < 322; i++) { // unset all keys
        keys[i] = 0;
    }
    // Allow keys to be held down
    SDL_EnableKeyRepeat(0,0);
}


/* Check each individual GameBoy key. Returns 1 if
 * the specified key is being held down, 0 otherwise */
int down_pressed()   { return keys[SDLK_DOWN];  }  
int up_pressed()     { return keys[SDLK_UP]; }
int left_pressed()   { return keys[SDLK_LEFT];}
int right_pressed()  { return keys[SDLK_RIGHT];} 
int a_pressed()      { return keys[SDLK_a]; }
int b_pressed()      { return keys[SDLK_s];}
int start_pressed()  { return keys[SDLK_RETURN]; }
int select_pressed() { return keys[SDLK_SPACE]; } 


/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}


/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
void update_keys() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                    // exit if the window is closed
                case SDL_QUIT:
                            exit(0);
                            break;
              
                case SDL_KEYDOWN: // Key pressed
                            keys[event.key.keysym.sym] = 1;
                            if (keys[SDLK_ESCAPE]) {exit(0);}
                            break;
                case SDL_KEYUP: //Key "unpressed"
                            keys[event.key.keysym.sym] = 0;
                            break;
            }
        } 
}









