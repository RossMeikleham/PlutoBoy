#include "../../non_core/graphics_out.h"
#include "../../non_core/logger.h"
#include "../../non_core/joypad.h"

#ifdef __APPLE__
    #include "TargetConditionals.h"
#endif

#if defined(_MSC_VER) || defined(__ANDROID__)
#include "SDL.h"
#else 
#include <SDL2/SDL.h>
#endif

#ifdef PSVITA
#include "../../non_core/menu.h"
#else
static SDL_Window *screen;
#endif

static SDL_Renderer *renderer;
static SDL_Texture *texture; 
static Uint32 *pixels;

static SDL_Texture *overlay_t;

static int screen_width;
static int screen_height;

static void init_vbutton_texture() {
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);    
    
    SDL_Surface *overlay = SDL_CreateRGBSurface(0, current.w, current.h, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
   
    // Draw DPad
    SDL_Rect left = {DPAD_LEFT_X, DPAD_LEFT_Y(current.h), DPAD_LEFT_W, DPAD_LEFT_H};
    SDL_Rect right = {DPAD_RIGHT_X, DPAD_RIGHT_Y(current.h), DPAD_RIGHT_W, DPAD_RIGHT_H};
    SDL_Rect up = {DPAD_UP_X, DPAD_UP_Y(current.h), DPAD_UP_W, DPAD_UP_H};
    SDL_Rect down = {DPAD_DOWN_X, DPAD_DOWN_Y(current.h), DPAD_DOWN_W, DPAD_DOWN_H};

    // Draw Start/Select
    SDL_Rect start = {START_X, START_Y(current.h), START_W, START_H};
    SDL_Rect select = {SELECT_X, SELECT_Y(current.h), SELECT_W, SELECT_H};

    // Draw A/B
    SDL_Rect a = {A_X(current.w), A_Y(current.h), A_W, A_H};
    SDL_Rect b = {B_X(current.w), B_Y(current.h), B_W, B_H};
    
    Uint32 b_color = SDL_MapRGB(overlay->format, 0x95, 0x8E, 0x8E);
 
    SDL_FillRect(overlay, &left, b_color);
    SDL_FillRect(overlay, &right, b_color);
    SDL_FillRect(overlay, &down, b_color);
    SDL_FillRect(overlay, &up, b_color);
    SDL_FillRect(overlay, &start, b_color);
    SDL_FillRect(overlay, &select, b_color);
    SDL_FillRect(overlay, &a, b_color);
    SDL_FillRect(overlay, &b, b_color);

    SDL_SetSurfaceBlendMode(overlay, SDL_BLENDMODE_BLEND);

    // Make overlay partially transparent
    SDL_SetSurfaceAlphaMod(overlay, 0x7F);
    overlay_t = SDL_CreateTextureFromSurface(renderer, overlay);
}

/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *p) {
    
    screen_width = win_x;
    screen_height = win_y;

    pixels = p;

    #ifndef PSVITA

    if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER/*| SDL_INIT_HAPTIC*/)==-1)) {
        log_message(LOG_ERROR, "Could not initialize SDL: %s.\n", SDL_GetError());
        return 0;
     }
    
    // Setup Window
    screen = SDL_CreateWindow("Plutoboy", 
                   SDL_WINDOWPOS_UNDEFINED, 
                   SDL_WINDOWPOS_UNDEFINED,
                   screen_width, screen_height, 
                   0); 
   
    if (screen == NULL) {
        log_message(LOG_ERROR, "Could not initialize SDL window: %s.\n", SDL_GetError());
        return 0;
    } 

    #endif

    // Setup Renderer
    renderer = SDL_CreateRenderer(screen, -1, 0);
    if (renderer == NULL) {
        log_message(LOG_ERROR, "Could not initialize SDL renderer: %s.\n", SDL_GetError());
        return 0;
    } 

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    // Setup texture for blitting the pixels
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_STREAMING, GB_PIXELS_X, GB_PIXELS_Y);


    if (texture == NULL) {
        log_message(LOG_ERROR, "Could not initialize SDL texture: %s.\n", SDL_GetError());
        return 0;
    }

    init_vbutton_texture();

    return 1;
}


/*  Update the screen output */
void draw_screen() {
    SDL_UpdateTexture(texture, NULL, pixels, GB_PIXELS_X * sizeof (Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
   
    // Render the virtual keyboard if using a mobile device 
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)  
    SDL_RenderCopy(renderer, overlay_t, NULL, NULL);
#endif

    SDL_RenderPresent(renderer);
}



