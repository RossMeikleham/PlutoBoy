#include "../../non_core/framerate.h"

#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

static uint64_t last_ticks;
static int framerate_times_ten;
static int count;
static uint64_t current_ticks = 0;


uint64_t get_timestamp_micro() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ull + tv.tv_usec;
}

//Assign Framerate in FPS and start counter
void start_framerate(int f) {
#ifndef EMSCRIPTEN
    last_ticks = get_timestamp_micro();
    framerate_times_ten = f;
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
    uint64_t estimated_ticks = last_ticks + (uint64_t)(10000000/framerate_times_ten);
    uint64_t framerate_ticks = (ticks_elapsed * framerate_times_ten) / 10;

    
    // If too fast we sleep for a certain amount of time
    // sleep might go over the time we want to wait
    // so attempt to come out of sleep early and use
    // cpu cycles to wait for the rest of the time
    if (framerate_ticks < 1000000) {
        uint64_t delay_time = 10000000/(framerate_times_ten) - ticks_elapsed;
        if (delay_time >= 5000) {
            //casting uint64_t into uint32_t, not really safe
            // but we will never be delaying for more than 1000ms which is well in range
                
            // WatchOS usleep unfortunately overshoots by a rediculous amount
            // before exiting sleep. Reducing the "/ 20" value to 1 will give
            // much more consistent framerate but at 100% CPU speed due to the spinlock
            // that comes after this
                usleep( (uint32_t) ((delay_time - 200) / 20));
            }
            // Spinlock til the delay time is met
            while(get_timestamp_micro() < estimated_ticks)
                  ;;
    }
   
    current_ticks = get_timestamp_micro();
    
    count = (count + 1) % 60;
       if (count == 0) {
           float fps = 1000000.0 / (current_ticks - last_ticks);
       }
       last_ticks = current_ticks;

    count = (count + 1) % 60;
    //if (count == 0) {
        //float fps = 1000000.0 / (current_ticks - last_ticks);
        //printf("fps %f\n", fps);
        
//        sprintf(title_buf, "Gameboy fps:%.2f",fps);
//        printf("Gameboy fps:%.2f\n",fps);
//        SDL_WM_SetCaption(title_buf,"");
   // }
    last_ticks = current_ticks;
}
#endif
