#ifndef GRAPHICS_OUT
#define GRAPHICS_OUT

#define GB_PIXELS_X 160 //Gameboy Pixel dimensions
#define GB_PIXELS_Y 144

#include <stdint.h>

/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *pixels);

/*  Update the screen output*/
void draw_screen();


#endif
