#include "../../non_core/graphics_out.h"
#include "../../non_core/logger.h"

#include <string.h>
#include <3ds.h>

// screen attr
#define TOPSCREEN_WIDTH  400
#define BOTTOMSCREEN_WIDTH 340
#define SCREEN_HEIGHT 240
#define SCREEN_BPP 16

#define GAME_PIXELS_X ((160 * 5) / 3)
#define GAME_PIXELS_Y ((144 * 5) / 3)

#define RATIO 1.5

#define START_X 120
#define START_Y 48

#define INNER_BORDER_WIDTH 30

//static uint32_t scaled_pixels[GAME_PIXELS_X * GAME_PIXELS_Y];

static uint32_t *pixels;
static int screen_width;
static int screen_height;

/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *p) {

    screen_width = TOPSCREEN_WIDTH;
    screen_height = SCREEN_HEIGHT;
    gfxFlushBuffers();
    gfxSwapBuffers();
    
    // Cleanout the Framebuffers
    pixels = p;
    uint8_t* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    for (int i = 0; i < 2; i++) {
        for (int y = 0; y < SCREEN_HEIGHT; y++)  {
            for (int x = 0; x < TOPSCREEN_WIDTH; x++) {
                int col = 0x00000000;
                memcpy(&fb[3 * (239 - y + (x * 240))], &col, 3);
}
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

/*
    uint32_t outer_border_color = 0x00FF00; // Purple
    uint32_t inner_border_color = 0x212327; 

    gfxSetDoubleBuffering(GFX_TOP, true);
   
    for (int i = 0; i < 2; i++) { 
        uint8_t* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < TOPSCREEN_WIDTH; x++) {
                uint32_t border_color;
                
                if ((x < 82 || x > (TOPSCREEN_WIDTH - 82))
                    || (y < 25 || y > (SCREEN_HEIGHT - 25))) {
                    border_color = outer_border_color;
                } else {
                    border_color = inner_border_color;
                }
                
                memcpy(&fb[3 * (239 - y + (x * 240))], &border_color, 3);
            }
        }

        // Remove Corners
        int depth = 1;
        for (int i = 0; i < depth; i++) {

            memcpy(&fb[3 * (239 - y + (x * 240))], &outer_border_color, 3);
        }

        gfxSwapBuffers();
    }
*/
    return 1;
}

/*  Update the screen output */
void draw_screen() {    

    gfxFlushBuffers();
    gfxSwapBuffers();

	//Wait for VBlank
    uint8_t* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    for (int y = 0; y < GB_PIXELS_Y; y++) {
        for (int x = 0; x < GB_PIXELS_X; x++) {
            uint32_t pixel = pixels[((y * GB_PIXELS_X) + x)];
            memcpy(&fb[3 * (239 - (y + START_Y) + ((x + START_X) * 240))], &pixel, 3);
        }
    }
}
/* Slow :(    
    uint8_t* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    for (int y = 0; y < GAME_PIXELS_Y; y++) {
        for (int x = 0; x < GAME_PIXELS_X; x++) {
            uint32_t pix_x_idx = (x * 3) / 5;
            uint32_t pix_y_idx = (y * 3) / 5;
            uint32_t pixel = pixels[(pix_y_idx * GB_PIXELS_X) + pix_x_idx];
            memcpy(&fb[3 * (239 - (y + START_Y) + ((x + START_X) * 240))], &pixel, 3);
        }
    }

*/
   
   
    
/*  static void handle_input() {

    if (!aptMainLoop()) {
        
    }

    hidScanInput();

}*/
