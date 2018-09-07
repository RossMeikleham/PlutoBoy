#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include <3ds.h>

extern PrintConsole fps_limiter_screen, fps_screen, battery_screen, debug_screen;

void show_battery();

void show_fps(float fps);

void show_fps_limiter();

#endif
