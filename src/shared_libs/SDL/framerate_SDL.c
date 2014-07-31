#include "SDL/SDL.h"
#include "stdlib.h"
#include "../../non_core/framerate.h"

static Uint32 last_ticks;
static int framerate;
static int count;
static int current_ticks = 0;

//Assign Framerate in FPS and start counter
void start_framerate(unsigned long f) {
    last_ticks = SDL_GetTicks();
    framerate = f;
    count = 0;
}

/* Check time elapsed after one frame, hold up
 * the program if not enough tim has elapsed */
void adjust_to_framerate() {

    char title_buf[100];

    current_ticks =  SDL_GetTicks();
    Uint32 ticks_elapsed = current_ticks - last_ticks;
    Uint32 framerate_ticks = ticks_elapsed * framerate;
    
    // If too fast
    if (framerate_ticks < 1000) {
        SDL_Delay((1000.0/framerate) - ticks_elapsed);
    } 
   
    current_ticks = SDL_GetTicks();
    count = (count + 1) % 60;
    if (count == 0) {
        double fps = 1000.0 / (current_ticks - last_ticks);
        snprintf(title_buf, 100, "Gameboy fps:%.2f",fps);
        SDL_WM_SetCaption(title_buf,"");
    }
    last_ticks = current_ticks;

}









