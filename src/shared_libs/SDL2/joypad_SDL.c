#include "../../non_core/joypad.h"

#ifdef _MSC_VER
#include "SDL.h"
#else 
#include <SDL2/SDL.h>
#endif

#include "stdlib.h"
#include "../../core/mmu/mbc.h"

typedef struct {
    int key_code; // Key code on the keyboard which
                       // maps to the given gameboy button
    int state; // 1 pressed, 0 unpressed
} button_state;

enum {UP = 0, DOWN, LEFT, RIGHT, A, B, START, SELECT};

button_state buttons[8];

#define TOTAL_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() { 
     
    buttons[UP].state = 0;
    buttons[UP].key_code = SDLK_UP;

    buttons[DOWN].state = 0;
    buttons[DOWN].key_code = SDLK_DOWN;

    buttons[LEFT].state = 0;
    buttons[LEFT].key_code = SDLK_LEFT;

    buttons[RIGHT].state = 0;
    buttons[RIGHT].key_code = SDLK_RIGHT;

    buttons[A].state = 0;
    buttons[A].key_code = SDLK_a;

    buttons[B].state = 0;
    buttons[B].key_code = SDLK_s;

    buttons[START].state = 0;
    buttons[START].key_code = SDLK_RETURN;
   
    buttons[SELECT].state = 0;
    buttons[SELECT].key_code = SDLK_SPACE;
     
    // Allow keys to be held down
    //SDL_EnableKeyRepeat(0,0);
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


static float last_touch_x = -1.0;
static float last_touch_y = -1.0;

void check_keys_pressed(float x, float y, int state) {
    if (x <= 0.5) {
        if (x <= 0.2 && y >= 0.2 && y <= 0.4) {
            buttons[LEFT].state = state;
        } else if (x > 0.2 && x < 0.4 && y > 0.6) {
            buttons[DOWN].state = state;
        } else if (x >= 0.4 && y >= 0.2 && y <= 0.4) {
            buttons[RIGHT].state = state;
        } else if (x > 0.2 && x < 0.4 && y <= 0.6) {
            buttons[UP].state = state;
        }
    } else {
        if (x <= 0.7 && y >= 0.2 && y <= 0.4) {
            buttons[A].state = state;
        } else if (x > 0.7 && x < 0.9 && y > 0.6) {
            buttons[START].state = state;
        } else if (x >= 0.9 && y >= 0.2 && y <= 0.4) {
            buttons[SELECT].state = state;
        } else if (x > 0.7 && x < 0.9 && y <= 0.6) {
            buttons[B].state = state;
        }
    }
}

void unset_keys() {
    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        buttons[i].state = 0;
    }
}

/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
void update_keys() {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                    // exit if the window is closed
                case SDL_QUIT:
                            write_SRAM();
                            exit(0);
                            break;
                case SDL_KEYDOWN: // Key pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        write_SRAM();
                        exit(0);
                    }

                    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
                            if (buttons[i].key_code == event.key.keysym.sym) {
                                buttons[i].state = 1;
                                break;
                            }
                        }
                        break;

                case SDL_KEYUP: //Key released
                    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
                            if (buttons[i].key_code == event.key.keysym.sym) {
                                buttons[i].state = 0;
                                break;
                            }
                        }
                    break;
                
                case SDL_FINGERDOWN:
                    check_keys_pressed(event.tfinger.x, event.tfinger.y, 1);
                    break;
                
                case SDL_FINGERUP:
                    check_keys_pressed(event.tfinger.x, event.tfinger.y, 0);
                    break;
            }
        }
}








