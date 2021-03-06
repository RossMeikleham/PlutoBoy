#include "../../non_core/graphics_out.h"
#include "../../non_core/logger.h"
#include "../../non_core/joypad.h"
#include "../../shared_libs/WatchOS/graphics_WatchOS_callbacks.h"

static void (* _Nonnull display_screen_callback)(const void* _Nonnull slf, const uint32_t * _Nonnull pixels) = NULL;
static uint32_t *pixels;
static const void* self;

/*  Initialise graphics and create win_x by win_y pixel
 *  screen. Keeps track of GB_PIXELS_Y by GB_PIXELS_X screen. 
 *  return  1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *p) {
    pixels = p;
    return 1;    
}

static inline void convert_pixels_to_sk_texture_pix() {
    
    for (int i = 0; i < GB_PIXELS_X  * GB_PIXELS_Y; i ++) {
        uint32_t cur_pix = pixels[i];
        uint32_t red = (cur_pix & 0xFF0000) >> 16;
        uint32_t green = (cur_pix & 0x00FF00);
        uint32_t blue = (cur_pix & 0x0000FF) << 16;
        pixels[i] = 0xFF000000 | blue | green | red;
    }
}


/*  Update the screen output */
void draw_screen() {
    convert_pixels_to_sk_texture_pix();
    display_screen_callback(self, pixels);
}


void setup_graphics_callbacks(const void* _Nonnull slf, const struct graphics_callbacks * _Nonnull graphics_callbacks) {
    self = slf;
    display_screen_callback = graphics_callbacks->display_screen;
}
