#include "../../non_core/framerate.h"

#include <stdint.h>

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellParameters.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>


static uint64_t last_ticks;
static int  framerate_times_ten;
static int count;
static uint64_t current_ticks = 0;

uint64_t get_timestamp_micro() {
	return 0;
    struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ull + tv.tv_usec; 
}

//Assign Framerate in FPS and start counter
void start_framerate(int f) {
    last_ticks = get_timestamp_micro();
    framerate_times_ten = f;
    count = 0;
}

/* Check time elapsed after one frame, hold up
 * the program if not enough tim has elapsed */
void adjust_to_framerate() {

    //char title_buf[100];

    current_ticks = get_timestamp_micro();
    uint64_t ticks_elapsed = current_ticks - last_ticks;
   // uint64_t estimated_ticks = last_ticks + (uint64_t)(1000 * (1000/framerate));
    //uint64_t framerate_ticks = ticks_elapsed * framerate;

    // Running on Bare Metal, just delay for the required cycles 
    gBS->Stall((UINTN)(10000000/(framerate_times_ten) - ticks_elapsed));
    /*if (framerate_ticks < 1000000) {        
	uint64_t delay_time = 1000000/framerate - ticks_elapsed;
	if (delay_time >= 5000) {
		//casting uint64_t into uint32_t, not really safe
		// but we will never be delaying for more than 1000ms which is well in range
 		SDL_Delay( (uint32_t) ((delay_time - 200)/1000));        
	}	
    while(get_timestamp_micro() < estimated_ticks)
  		;;
	
    } 
   */
    current_ticks = get_timestamp_micro();
    
    count = (count + 1) % 60;
    /*if (count == 0) {
	//printf("speed %u\n", current_ticks - last_ticks);
    //    float fps = 1000000.0 / (current_ticks - last_ticks);
        
//        sprintf(title_buf, "Gameboy fps:%.2f",fps);
//        printf("Gameboy fps:%.2f\n",fps);
//        SDL_WM_SetCaption(title_buf,"");
    }*/
    last_ticks = current_ticks;
}
