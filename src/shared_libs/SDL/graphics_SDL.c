/*
 * =====================================================================================
 *
 *       Filename:  graphics_SDL.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/14 10:53:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "../../non_core/graphics_out.h"
#include "SDL/SDL.h"

#include <stdio.h>


typedef int (*Int_2D )[GB_PIXELS_X];

static Uint32 colors[4];
static SDL_Surface *screen;
static Int_2D screen_buffer;// (*screen_buffer)[GB_PIXELS_X];
static int screen_width;
static int screen_height;


/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, const int *pixels) {
       
    screen_width = win_x;
    screen_height = win_y;

    screen_buffer = (int (*)[GB_PIXELS_X])pixels;
    
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        return 0;
    }

    SDL_WM_SetCaption("Gameboy","");
    screen = SDL_SetVideoMode(screen_width, screen_height, 32 ,SDL_DOUBLEBUF);

   // cols[0] = SDL_MapRGB(screen->format, 255, 255, 255); /* White */
   // cols[1] = SDL_MapRGB(screen->format, 170, 170, 170); /* Light Grey */
   // cols[2] = SDL_MapRGB(screen->format, 85, 85, 85); /* Dark Grey */
   // cols[3] = SDL_MapRGB(screen->format, 0, 0, 0); /* Black */

   // cols[0] = SDL_MapRGB(screen->format, 155, 187, 14);
   // cols[1] = SDL_MapRGB(screen->format, 115, 160, 103);
   // cols[2] = SDL_MapRGB(screen->format, 53, 98, 55);
   // cols[3] = SDL_MapRGB(screen->format, 15, 56, 14);

    colors[0] = SDL_MapRGB(screen->format, 255, 255, 255);
    colors[1] = SDL_MapRGB(screen->format, 136, 192, 112);
    colors[2] = SDL_MapRGB(screen->format, 48,  104, 80);
    colors[3] = SDL_MapRGB(screen->format, 8, 24, 32);

    return 1;

}


inline static void fill_rect(int x, int y, int w, int h, Uint32 color)
{
    
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(screen, &rect, color);
}


inline static void draw_pix(Uint32 color, int x, int y)
{
    int width_inc = screen_width/GB_PIXELS_X;
    int height_inc = screen_height/GB_PIXELS_Y;
    fill_rect(x*width_inc,y*height_inc,width_inc,height_inc, color);
        
}

void draw_screen() {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            draw_pix(colors[screen_buffer[y][x]], x, y);
        }
    } 
    
    SDL_Flip(screen);   
}




