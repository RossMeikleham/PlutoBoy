#ifndef GRAPHICS_H
#define GRAPHICS_H


#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144


int init_gfx();
void draw_row();
void output_screen();

void set_oam_mem(uint16_t addr, uint8_t val);
uint8_t get_oam_mem(uint16_t addr);

#endif /* GRAPHICS_H */

