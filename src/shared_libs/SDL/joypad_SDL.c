/*
 * =====================================================================================
 *
 *       Filename:  joypad_SDL.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19/07/14 13:20:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "SDL/SDL.h"
#include "stdlib.h"
#include "../../non_core/joypad.h"

int keys[322];  // 322 SDL keys



void init_joypad() {

    for(int i = 0; i < 322; i++) { // unset all keys
        keys[i] = 0;
    }
    // Allow keys to be held down
    SDL_EnableKeyRepeat(0,0);
}


int down_pressed()   { return keys[SDLK_DOWN];  }  
int up_pressed()     { return keys[SDLK_UP]; }
int left_pressed()   { return keys[SDLK_LEFT];}
int right_pressed()  { return keys[SDLK_RIGHT];} 
int a_pressed()      { return keys[SDLK_a]; }
int b_pressed()      { return keys[SDLK_s];}
int start_pressed()  { return keys[SDLK_RETURN]; }
int select_pressed() { return keys[SDLK_SPACE]; } 

int key_pressed() {

    return down_pressed() || up_pressed() || left_pressed() || right_pressed()
    || a_pressed() || b_pressed() || start_pressed() || select_pressed();
}


void update_keys() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                // exit if the window is closed
            case SDL_QUIT:
                exit(0);
                break;
                // check for keypresses
            case SDL_KEYDOWN:
                keys[event.key.keysym.sym] = 1;
                if (keys[SDLK_ESCAPE]) {exit(0);}
                break;
            case SDL_KEYUP:
                keys[event.key.keysym.sym] = 0;
                break;
            default:
                break;
            }
        } 
}









