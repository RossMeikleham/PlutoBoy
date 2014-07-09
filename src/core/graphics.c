/*
* ====================================================================================
*   Filenamme: graphics.c
*
*   Description:
*
*   Created: 18/03/14 15:48:01
*
*   Author: Ross Meikleham
*
* =====================================================================================
*/

#include "cpu.h"
#include <stdio.h>
#include "SDL/SDL.h"
#include "memory.h"
#include "memory_layout.h"
#include "stdint.h"
#include "graphics.h"
#include "IO.h"


static Uint32 cols[4];
static SDL_Surface *screen;
static SDL_Event event;

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
    cols[3] = SDL_MapRGB(screen->format, 0, 0, 0); /* Black */

    return 1;
}


/* Return Tile 8x8 pixels from vram given tile type */
Tile get_tile(uint8_t tile_no, TileType tile_type) {

    uint16_t tile_set_mem =
        tile_type == TYPE0 ? TILE_SET_0_START : TILE_SET_1_START;

    uint16_t tile_start_loc = tile_set_mem + (tile_no * 16);
    uint8_t tile_byte_0, tile_byte_1;
    Tile tile;

    for (int i = 0; i < 8; i++) {
        /* Each row of 8 pixels in a tile made of 2 bytes */
        tile_byte_0 = get_mem(tile_start_loc + (2 * i));
        tile_byte_1 = get_mem(tile_start_loc + (2 * i) + 1);
    
        /* Obtain the colour for each pixel in the row.
* for bit n in the row, colour is calculated by
* the values of the nth bit of each two tile bytes */
        for (int j = 0; j < 8; j++) {
            int bit_byte_0 = tile_byte_0 & (0x80 >> j);
            int bit_byte_1 = tile_byte_1 & (0x80 >> j);
            
            if (! (bit_byte_0 || bit_byte_1)) {
                tile.colour[i][j] = WHITE;
            } else if (bit_byte_0 && bit_byte_1) {
                tile.colour[i][j] = BLACK;
            } else if (bit_byte_0 && !bit_byte_1) {
                tile.colour[i][j] = GREY;
            } else { /* bit 0 unset and bit 1 set */
                tile.colour[i][j] = DARK_GREY;
            }

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
    uint8_t y = get_win_y_pos() + row; /* overlaps if overflow */
    uint8_t x = get_win_x_pos();
    lcd_ctrl_t lcd_ctrl;
    lcd_ctrl = get_lcd_control();
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






/* Tile type 0 supplied as unsigned int
* ranging from 0 to 255 */
Tile get_tile_0(uint8_t tile_no){
    return get_tile(tile_no, TYPE0);
}

/* Tile type 1 supplied as signed int
* ranging from -128 to 127 */
Tile get_tile_1(int8_t tile_no) {
    int i_tile_no = tile_no + 128;
    uint8_t u_tile_no = i_tile_no;
    return get_tile(u_tile_no, TYPE1);
}





/* 0 < x,y < 32 */
void draw_tile(uint8_t tile_no, uint8_t row, uint8_t col, TileType tile_type) {

    int startx = row * 8;
    int starty = col * 8;

    /* Calculate tile from VRAM */
    Tile tile = get_tile(tile_no, tile_type);

    /* Draw each pixel in the tile */
    for (int y = starty; y < starty + 8; y++) {
        for (int x = startx; x < startx + 8; x++) {
            draw_pix(tile.colour[y-starty][x-startx], x, y);
        }
    }

    update_screen();
}

/* Draw tile 0-127 from 0x8000 - 0x8FFF */
void draw_tile_0(uint8_t tile_no, uint8_t x, uint8_t y) {
    draw_tile(tile_no, x,y, TYPE0);
}

/* Draw tile -128 to 127 from 0x8800 - 0x97FF */
void draw_tile_1(int8_t tile_no, uint8_t x, uint8_t y) {
    int16_t i_tile_no = tile_no + 128;
    uint8_t u_tile_no = i_tile_no;
    draw_tile(u_tile_no, x, y, TYPE1);
}


/* Obtain the 32x32 background */
Background get_background(TileType tile_type) {

    uint16_t bg_map_set_mem =
        tile_type == TYPE0 ? BG_MAP_DATA0_START : BG_MAP_DATA1_START;
    
    Background background;
    background.type = tile_type;
        
    for (uint16_t y = 0; y < 32; y++) {
        for(uint16_t x = 0; x < 32; x++) {
            if (tile_type == TYPE0) {
                background.tiles[y][x] = get_mem(bg_map_set_mem);
            } else if (tile_type == TYPE1) {
                /* Type 1 is signed from -127 to 128 so need to convert */
                uint8_t val = get_mem(bg_map_set_mem);
                val = val < 128 ? 128 + val : (val - 128);
                background.tiles[y][x] = val;
                
            } else {
                fprintf(stderr, "error, invalid tile_type entered\n");
                exit(1);
            }
        }
    }

    return background;
}


void draw_background(TileType tile_type) {
    Background background = get_background(tile_type);

    for (uint8_t y = 0; y < 32; y++) {
        for(uint8_t x = 0; x < 32; x++) {
            uint8_t tile_no = background.tiles[y][x];
            printf("x:%d y:%d tNo:%d\n",x,y,tile_no);
            draw_tile(tile_no, x,y, tile_type);
        }
    }
}


void draw_background_0() {
    draw_background(TYPE0);
}

void draw_background_1() {
    draw_background(TYPE1);
}

/* Draw sprites/OAM from vram */
void draw_sprites() {
    
    /* Sprite pattern table 0x8000 - 0x8FFF */
    for (int i = 0; i < 40; i++) {
        
        uint8_t y_pos = SPRITE_ATTRIBUTE_TABLE_START + (4*i);
        uint8_t x_pos = SPRITE_ATTRIBUTE_TABLE_START + (4*i) + 1;
        uint8_t tile_no = SPRITE_ATTRIBUTE_TABLE_START + (4*i) + 2;
        uint8_t attributes = SPRITE_ATTRIBUTE_TABLE_START + (4*i) + 3;

        draw_tile_0(tile_no, x_pos, y_pos);
    }
}


