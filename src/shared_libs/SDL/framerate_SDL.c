#include "SDL/SDL.h"
#include "../../non_core/framerate.h"

static Uint32 last_ticks;
static int framerate;


//Assign Framerate in FPS and start counter
void start_framerate(unsigned long f) {
    last_ticks = SDL_GetTicks();
    framerate = f;
}

/* Check time elapsed after one frame, hold up
 * the program if not enough tim has elapsed */
void adjust_to_framerate() {
 
    Uint32 current_ticks = SDL_GetTicks();
    Uint32 ticks_elapsed = current_ticks - last_ticks;
    Uint32 framerate_ticks = ticks_elapsed * framerate;
    
    // If too fast
    if (framerate_ticks < 1000) {
        SDL_Delay((1000/framerate) - ticks_elapsed);
    }
   
    last_ticks = SDL_GetTicks();
}









