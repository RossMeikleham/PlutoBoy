#include "../../non_core/joypad.h"

#if defined(_MSC_VER) || defined(__ANDROID__)
#include "SDL.h"
#else 
#include <SDL2/SDL.h>
#endif

#include "stdlib.h"
#include "../../core/mmu/mbc.h"
#include "../../non_core/logger.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
} b_rect;

typedef struct {
    int key_code; // Key code on the keyboard which
                       // maps to the given gameboy button
    int state; // 1 pressed, 0 unpressed
    b_rect rect;
} button_state;


enum {UP = 0, DOWN, LEFT, RIGHT, A, B, START, SELECT};

button_state buttons[8];

#define TOTAL_BUTTONS (sizeof(buttons)/sizeof(buttons[0]))

static SDL_DisplayMode current;
static SDL_Haptic *haptic;
static int rumble_on = 0; // If rumble is activated

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() { 
   
    rumble_on = 0;

    // Attempt to setup Haptic Feedback
    haptic = SDL_HapticOpen(0);
    if (haptic == NULL) {
        log_message(LOG_WARN, "Unable to open haptic device %s\n", SDL_GetError());
    } else {
        if (haptic != NULL && SDL_HapticRumbleInit(haptic) != 0) {
            rumble_on = 1;
        } else {
            log_message(LOG_WARN, "Unable to initialize rumble %s\n", SDL_GetError());
        }
    } 
    
    // Setup buttons 
    SDL_GetCurrentDisplayMode(0, &current);    
     
    buttons[UP].state = 0;
    buttons[UP].key_code = SDLK_UP;
    b_rect rect_u = {DPAD_UP_X, DPAD_UP_Y(current.h), DPAD_UP_W, DPAD_UP_H};
    buttons[UP].rect = rect_u;

    buttons[DOWN].state = 0;
    buttons[DOWN].key_code = SDLK_DOWN;
    b_rect rect_d = {DPAD_DOWN_X, DPAD_DOWN_Y(current.h), DPAD_DOWN_W, DPAD_DOWN_H};
    buttons[DOWN].rect = rect_d; 

    buttons[LEFT].state = 0;
    buttons[LEFT].key_code = SDLK_LEFT;
    b_rect rect_l = {DPAD_LEFT_X, DPAD_LEFT_Y(current.h), DPAD_LEFT_W, DPAD_LEFT_H};
    buttons[LEFT].rect = rect_l; 

    buttons[RIGHT].state = 0;
    buttons[RIGHT].key_code = SDLK_RIGHT;
    b_rect rect_r = {DPAD_RIGHT_X, DPAD_RIGHT_Y(current.h), DPAD_RIGHT_W, DPAD_RIGHT_H};
    buttons[RIGHT].rect = rect_r; 

    buttons[A].state = 0;
    buttons[A].key_code = SDLK_a;
    b_rect rect_a = {A_X(current.w), A_Y(current.h), A_W, A_H};
    buttons[A].rect = rect_a; 

    buttons[B].state = 0;
    buttons[B].key_code = SDLK_s;
    b_rect rect_b = {B_X(current.w), B_Y(current.h), B_W, B_H};
    buttons[B].rect = rect_b; 

    buttons[START].state = 0;
    buttons[START].key_code = SDLK_RETURN;
    b_rect rect_st = {START_X, START_Y(current.h), START_W, START_H};
    buttons[START].rect = rect_st; 
   
    buttons[SELECT].state = 0;
    buttons[SELECT].key_code = SDLK_SPACE;
    b_rect rect_se = {SELECT_X, SELECT_Y(current.h), SELECT_W, SELECT_H};
    buttons[SELECT].rect = rect_se; 
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
    float p_x = x * current.w;
    float p_y = y * current.h;
    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        if (p_x >= buttons[i].rect.x && 
                p_x <= buttons[i].rect.x + buttons[i].rect.w &&
                p_y >= buttons[i].rect.y &&
                p_y <= buttons[i].rect.y + buttons[i].rect.h) {
            
            // If activating button, send rumble feedback
            if (rumble_on && !buttons[i].state && state) {
               // SDL_HapticRumblePlay(haptic, 0.5, 100);
            }
            buttons[i].state = state;
            
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

                // Assume only one finger can be on a button at a time considering
                // the size of the buttons. It would be more accurate in the future
                // to keep track of finger ids for each button 
                case SDL_FINGERMOTION:
                    // Unset any buttons pushed with the finger before the motion
                    check_keys_pressed(event.tfinger.x - event.tfinger.dx,
                                       event.tfinger.y - event.tfinger.dy, 0);     
                        
                    // Set buttons currently being pushed by the finger
                    check_keys_pressed(event.tfinger.x, event.tfinger.y, 1);
                    break;
            }                   
        }
}








