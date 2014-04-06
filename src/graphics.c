/*
 * =====================================================================================
 *
 *       Filename:  graphics.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/03/14 15:48:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ross Meikleham 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "cpu.h"
#include <stdio.h>
#include "SDL/SDL.h"
#include "memory.h"
#include "stdint.h"
#include "graphics.h"
#include "IO.h"

Uint32 cols[3];
SDL_Surface *screen;
SDL_Event event;

int VBLANK_ENABLED; 


int Screen_Width = SCREEN_WIDTH * 2;
int Screen_Height = SCREEN_HEIGHT * 2;


uint8_t bit_mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20
    , 0x40, 0x80};


int init_gfx() {

    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) {
    	printf("Could not initialize SDL: %s.\n", SDL_GetError());
    	return 0;
    }

    SDL_WM_SetCaption("Gameboy","");
    screen = SDL_SetVideoMode(Screen_Width,Screen_Height, 32 ,SDL_DOUBLEBUF);
    SDL_EnableKeyRepeat(0,0);

    cols[0] = SDL_MapRGB(screen->format, 255, 255, 255); /* White */
    cols[1] = SDL_MapRGB(screen->format, 170, 170, 170); /* Light Grey */
    cols[2] = SDL_MapRGB(screen->format, 85, 85, 85); /* Dark Grey */ 
    cols[3] = SDL_MapRGB(screen->format, 0, 0, 0); /*  Black */

    return 1;
}



Tile get_tile(uint8_t tile_no, TileType tile_type) {

    uint16_t tile_set_mem =  
        tile_type == TYPE0 ? TILE_SET_0_START : TILE_SET_1_START;

    uint16_t tile_start_loc = tile_set_mem + (tile_no * 16);
    int i, j;
    uint8_t tile_byte_0, tile_byte_1;
    Tile tile;
    for (i = 0; i < 8; i++) {
        tile_byte_0 = get_mem(tile_start_loc);
        tile_byte_1 = get_mem(tile_start_loc+1);

        /*  Obtain the colour for each pixel in the row
         *  for bit n in the row, colour is calculated by
         *  the values of the nth bit of each two tile bytes */
        for (j = 0; j < 8; j++) {
            tile.colour[i][j] = (((tile_byte_1 & bit_mask[i]) >> i)
            << 1)
            | (((tile_byte_0 & bit_mask[i]) >> i));
        }

    }

    return tile;
}




void fill_rect(int x, int y, int w, int h, Uint32 color) 
{
    SDL_Rect rect = {x,y,w,h};
    SDL_FillRect(screen, &rect, color);
}



void draw_pix(Colour colour, int x, int y)
{
    int width_inc = Screen_Width/SCREEN_WIDTH;
    int height_inc = Screen_Height/SCREEN_HEIGHT;

    fill_rect(x*width_inc,y*height_inc,width_inc,height_inc,cols[colour]);
        
}

void update_screen()
{
    SDL_Flip(screen);
}




void draw_row(uint8_t row) {

    uint8_t i, x_pos;
    uint8_t y = get_win_y_pos() + row; /*  overlaps if overflow */
    uint8_t x = get_win_x_pos();
    lcd_ctrl_t lcd_ctrl;
    lcd_ctrl = get_lcd_control();
    Colour colour;
    Tile tile;

    uint16_t current = lcd_ctrl.bg_win_tile == 0 ? 0x9800 : 0x9C00;
    

    for (x_pos = x; x_pos-x < SCREEN_WIDTH/8; x_pos++) {
        tile = get_tile(x , !current);
        for (i = 0; i < 8; i++) {
            draw_pix(tile.colour[i][0], (x_pos*8)+i ,y);
        }
    }

    update_screen();
}






/*  Tile type 0 supplied as unsigned int
 *  ranging from 0 to 255 */
Tile get_tile_0(uint8_t tile_no){
    return get_tile(tile_no, TYPE0);
}

/*  Tile type 1 supplied as signed int
 *  ranging from -128 to 127 */
Tile get_tile_1(int8_t tile_no) {
    uint8_t u_tile_no = tile_no + 128;
    return get_tile(u_tile_no, TYPE1);
}





