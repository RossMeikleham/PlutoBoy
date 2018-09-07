#include <3ds.h>
#include <stdio.h>
#include "../../non_core/framerate.h"

PrintConsole debug_screen, battery_screen, fps_screen, fps_limiter_screen;

void show_battery() {

    consoleSelect(&battery_screen);
	
	uint8_t battery_percent = 0;
	
	if (R_SUCCEEDED(MCUHWC_GetBatteryLevel(&battery_percent))) {
	
		char *color;
		char *old_color = "\x1b[0m";	
	
		if (battery_percent > 50) { // GREEN
    		color = "\x1b[32m";
		} else if (battery_percent > 15) { // YELLOW
			color = "[\x1b[33m";
		} else { // RED
			color = "[\x1b[31m"; 
		}
		
		printf("Battery:%s%d%%%s\n", color, battery_percent, old_color);
	};
 
	consoleSelect(&debug_screen);
}

void show_fps(float fps) {

	consoleSelect(&fps_screen);
	
	char *color;
	char *old_color = "\x1b[0m";	

	int fps_i = (int)fps;

	if (fps_i > 55) { // GREEN
    	color = "\x1b[32m";
	} else if (fps_i > 40) { // YELLOW
		color = "\x1b[33m";
	} else { // RED
		color = "\x1b[31m"; 
	}

	printf("FPS:%s%.2f%s\n", color,fps, old_color); 
	consoleSelect(&debug_screen);
}

void show_fps_limiter() {
	const char * const limiter_off = "\x1b[31mOFF\x1b[0m";
	const char * const limiter_on = "\x1b[32mON\x1b[0m";

	consoleSelect(&fps_limiter_screen);
	printf("FPS Limiter:%s\n", limiter ? limiter_on : limiter_off);
	consoleSelect(&debug_screen);
}
extern PrintConsole fps_limiter_screen, fps_screen, battery_screen, debug_screen;
//printf("Gameboy fps:%.2f\n",fps);
