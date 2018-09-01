#ifndef THREE_DS
#include "SDL/SDL.h"
#include "SDL.h"
#include "stdlib.h"
#include "../../non_core/joypad.h"
#include "../../core/mmu/mbc.h"
#include "../../non_core/logger.h"

static int keys[2000];  

typedef enum {TRIANGLE, CIRCLE, CROSS, SQUARE, LTRIGGER, RTRIGGER,
             DOWN, LEFT, UP, RIGHT, SELECT, START, HOME, CTRL_HOLD} PSP_Button;

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() {
    
    if((SDL_Init(SDL_INIT_JOYSTICK )!=0)) {
        log_message(LOG_ERROR, "Could not initialize Joypad: %s.\n", SDL_GetError());
        return;
     } else {
        log_message(LOG_INFO, "Joypad initialized\n"); 
    } 

    for(int i = 0; i < 322; i++) { // unset all keys
        keys[i] = 0;
    }
    // Allow keys to be held down
    SDL_EnableKeyRepeat(0,0);
}

#ifdef PSP
int down_pressed()   { return keys[DOWN]; }  
int up_pressed()     { return keys[UP]; }
int left_pressed()   { return keys[LEFT]; }
int right_pressed()  { return keys[RIGHT]; } 
int a_pressed()      { return keys[CROSS]; }
int b_pressed()      { return keys[CIRCLE]; }
int start_pressed()  { return keys[START]; }
int select_pressed() { return keys[SELECT]; } 

#elif defined(THREE_DS)
int down_pressed()   { return keys[SDLK_DOWN];  }  
int up_pressed()     { return keys[SDLK_UP]; }
int left_pressed()   { return keys[SDLK_LEFT];}
int right_pressed()  { return keys[SDLK_RIGHT];} 
int a_pressed()      { return keys[SDLK_a]; }
int b_pressed()      { return keys[SDLK_b];}
int start_pressed()  { return keys[SDLK_RETURN]; }
int select_pressed() { return keys[SDLK_ESCAPE]; } 

#else 
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
#endif


/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}


/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
int update_keys() {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                    // exit if the window is closed
                case SDL_QUIT:
                            write_SRAM();
                            return 1;
                            break;
#ifndef PSP
                case SDL_KEYDOWN: // Key pressed
                    keys[event.key.keysym.sym] = 1;
                    if (keys[SDLK_ESCAPE]) {
                        write_SRAM();
                        return 1;
                    }
                        break;

                case SDL_KEYUP: //Key "unpressed"
                    keys[event.key.keysym.sym] = 0;
                    break;
#endif

#ifdef PSP
                  
                case SDL_JOYBUTTONDOWN:
                    keys[event.jbutton.button] = 1; 
                    break;
                case SDL_JOYBUTTONUP:
                    keys[event.jbutton.button] = 0;
                    break;
#endif
                
             }
        }
        return 0;
}

#endif
