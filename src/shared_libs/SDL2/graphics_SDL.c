#include "../../non_core/graphics_out.h"
#include "../../non_core/logger.h"

#include <SDL2/SDL.h>

static SDL_Window *screen;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static Uint32 *pixels;

static int screen_width;
static int screen_height;

/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *p) {
    
    screen_width = win_x;
    screen_height = win_y;

    pixels = p;

    if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK )==-1)) {
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
                    SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);

    if (texture == NULL) {
        log_message(LOG_ERROR, "Could not initialize SDL texture: %s.\n", SDL_GetError());
        return 0;
    }

    return 1;
}


/*  Update the screen output */
void draw_screen() {
    SDL_UpdateTexture(texture, NULL, pixels, GB_PIXELS_X * sizeof (Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}



