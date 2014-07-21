/*
 * =====================================================================================
 *
 *       Filename:  framerate_SDL.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/07/14 12:36:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "SDL/SDL.h"
#include "../../non_core/framerate.h"

Uint32 last_ticks;
int framerate;

void start_framerate(unsigned long f) {
    last_ticks = SDL_GetTicks();
    framerate = f;
}

void adjust_to_framerate() {
 
    Uint32 current_ticks = SDL_GetTicks();
    Uint32 ticks_elapsed = current_ticks - last_ticks;
    Uint32 framerate_ticks = ticks_elapsed * framerate;
    
    // If too fast
    if (framerate_ticks < 1000) {
        SDL_Delay((1000/framerate) - ticks_elapsed);
    }
   
    last_ticks = current_ticks;
}









