#include "../../non_core/framerate.h"

#if defined(_MSC_VER) || defined(__ANDROID__)
#include "SDL.h"
#else 
#include <SDL2/SDL.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include "stdlib.h"

#ifndef EMSCRIPTEN
static uint64_t last_ticks;
static int framerate_times_ten;
static int count;
static uint64_t current_ticks = 0;
#endif

#ifdef _WIN32
#include <windows.h>
#include <time.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

static struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag = 0;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		tmpres /= 10;  /*convert into microseconds*/
		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}



#elif !defined(EMSCRIPTEN)
#include <sys/time.h>
#endif

#ifndef EMSCRIPTEN
uint64_t get_timestamp_micro() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ull + tv.tv_usec;
}
#endif


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
    uint64_t estimated_ticks = last_ticks + (uint64_t)(1000 * (10000/framerate_times_ten));
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
        
//        sprintf(title_buf, "Gameboy fps:%.2f",fps);
//        printf("Gameboy fps:%.2f\n",fps);
//        SDL_WM_SetCaption(title_buf,"");
    }
    last_ticks = current_ticks;
#endif
}









