#include "SDL.h"
#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"
#include "../../non_core/framerate.h"

#ifndef EMSCRIPTEN
static uint64_t last_ticks;
static float framerate;
static int count;
static uint64_t current_ticks = 0;
#endif

#ifdef _WIN32
#include <windows.h>

int gettimeofday( struct timeval *tv, struct timezone *tz ) {
    time_t rawtime;

    time(&rawtime);
    tv->tv_sec = (long)rawtime;

    LARGE_INTEGER tickPerSecond;
    LARGE_INTEGER tick; // a point in time

    QueryPerformanceFrequency(&tickPerSecond);

    QueryPerformanceCounter(&tick);

    tv->tv_usec = (tick.QuadPart % tickPerSecond.QuadPart);

    return 0;
}

#elif !defined(EMSCRIPTEN)
#include <sys/time.h>
#endif

uint64_t get_timestamp_micro() {
#ifndef EMSCRIPTEN
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ull + tv.tv_usec;
#endif
}

//Assign Framerate in FPS and start counter
void start_framerate(float f) {
#ifndef EMSCRIPTEN
    last_ticks = get_timestamp_micro();
    framerate = f;
    count = 0;
#endif
}

/* Check time elapsed after one frame, hold up
 * the program if not enough tim has elapsed */
void adjust_to_framerate() {
// EMSCRIPTEN had its own ways to run frames at certain FPS
#ifndef EMSCRIPTEN 

    char title_buf[100];

    current_ticks = get_timestamp_micro();
    uint64_t ticks_elapsed = current_ticks - last_ticks;
    uint64_t estimated_ticks = last_ticks + (uint64_t)(1000 * (1000/framerate));
    uint64_t framerate_ticks = ticks_elapsed * framerate;
    
    // If too fast we sleep for a certain amount of time
    // sleep might go over the time we want to wait
    // so attempt to come out of sleep early and use 
    // cpu cycles to wait for the rest of the time
    if (framerate_ticks < 1000000) {        
	uint64_t delay_time = 1000000/framerate - ticks_elapsed;
	if (delay_time >= 10000) {
		//casting uint64_t into uint32_t, not really safe
		// but we will never be delaying for more than 1000ms which is well in range
 		SDL_Delay( (uint32_t) ((delay_time - 200)/1000));        
	}	
    while(get_timestamp_micro() < estimated_ticks)
  		;;
	
    } 
   
    current_ticks = get_timestamp_micro();
    
    count = (count + 1) % 60;
    if (count == 0) {
	//printf("speed %u\n", current_ticks - last_ticks);
        float fps = 1000000.0 / (current_ticks - last_ticks);
        sprintf(title_buf, "Gameboy fps:%.2f",fps);
        SDL_WM_SetCaption(title_buf,"");
    }
    last_ticks = current_ticks;
#endif
}









