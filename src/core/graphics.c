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
#include <stdint.h>
#include "graphics.h"
#include "IO.h"
#include <stdlib.h>
#include <unistd.h>

static Uint32 cols[4];
static SDL_Surface *screen;

Uint32 screen_buffer[144][160];


int Screen_Width = SCREEN_WIDTH ;
int Screen_Height = SCREEN_HEIGHT ;



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



static void fill_rect(int x, int y, int w, int h, Uint32 color)
{
    
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(screen, &rect, color);
}



static void draw_pix(Uint32 color, int y, int x)
{
    int width_inc = Screen_Width/SCREEN_WIDTH;
    int height_inc = Screen_Height/SCREEN_HEIGHT;
    fill_rect(x*width_inc,y*height_inc,width_inc,height_inc, color);
        
}




static void draw_sprite_row() {
   
    // 8x16 or 8x8
    int8_t lcd_ctrl = get_mem(LCDC_REG);
    int height = lcd_ctrl & 0x4 ? 16 : 8;
    int window_on = lcd_ctrl & BIT_5;
    uint8_t obp_0 = get_mem(OBP0_REG); 
    uint8_t obp_1 = get_mem(OBP1_REG); 
    uint8_t row = get_mem(LY_REG);
    int palletes[2][4];

    //Calculate both color palletes
    palletes[0][0] =  obp_0  & 0x3;
    palletes[0][1] = (obp_0 >> 2) & 0x3;
    palletes[0][2] = (obp_0 >> 4) & 0x3;
    palletes[0][3] = (obp_0 >> 6) & 0x3;

    palletes[1][0] = obp_1  & 0x3;
    palletes[1][1] = (obp_1 >> 2) & 0x3;
    palletes[1][2] = (obp_1 >> 4) & 0x3;
    palletes[1][3] = (obp_1 >> 6) & 0x3;

    //40 Sprites
    for(int i = 0; i < 40; i++) {

        uint8_t y_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4)) - 16;
        uint8_t x_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 1) - 8;
        uint8_t tile_no = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 2);
        uint8_t attributes = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 3);

        if (height == 16) {
            tile_no &= ~0x1;
        }                           
        
        //If sprite doesn't intersect current line, no need to draw
        if (y_pos > row || row > y_pos + height) {
            continue;
        } 
       
        int x_flip = attributes & BIT_5;
        int y_flip = attributes & BIT_6;

        uint16_t tile_loc = TILE_SET_0_START + (tile_no * 16);

        // Obtain row of sprite to draw, if sprite is flipped
        // need to obtain row relative to bottom of sprite
        uint8_t line = (!y_flip) ? row - y_pos : y_pos - row;
        uint16_t line_offset = 2 * line;
        uint8_t high_byte = get_mem(tile_loc + line_offset);
        uint8_t low_byte =  get_mem(tile_loc + line_offset + 1);
        int pal_no = (attributes & BIT_4) ? 1 : 0;
        
        // Draw all pixels in current line of sprite
        for (int x = 0; x < 8; x++) {
            int bit_pos = (x_flip) ? x : 7 - x;
            int bit_0 = (low_byte >> bit_pos) & 0x1;
            int bit_1 = (high_byte >> bit_pos) & 0x1;
            uint8_t color_id = (bit_0 << 1) | bit_1;
            int priority  = !(attributes & 0x80);

            // draw if pixel is not transparent and either in foreground 
            // or window disabled and 
            
            uint8_t final_color_id = palletes[pal_no][color_id];
            if (priority) {
                if (screen_buffer[row][x_pos + x] != 0 && color_id != 0) {
                   screen_buffer[row][x_pos + x] = cols[final_color_id];
                }               
            } else  {
                if (color_id != 0) {
                   screen_buffer[row][x_pos + x] = cols[final_color_id];
                }
            }     
             
        }
    }
}



static void draw_tile_window_row(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
   
    uint8_t bgp = get_mem(BGP_REF);
    int pallete[4];
    //Calculate color pallete
    pallete[0] =  bgp  & 0x3;
    pallete[1] = (bgp >> 2) & 0x3;
    pallete[2] = (bgp >> 4) & 0x3;
    pallete[3] = (bgp >> 6) & 0x3;

    uint8_t y_pos = row - get_win_y_pos(); // Get line 0 - 255 being drawn    
    uint16_t tile_row = (y_pos / 8); // Get row 0 - 31 of tile
    uint8_t win_x = get_win_x_pos() - 7;
    uint8_t scroll_x = get_mem(SCROLL_X_REG);

    // 160 pixel row, 20 tiles, 8 pixel row per tile
    for (unsigned int i = 0; i < 160; i+=8) {
       
        uint8_t x_pos = i >= win_x ? (i - win_x) : (i + scroll_x);

        uint8_t tile_col = (x_pos) >> 3;
        uint8_t tile_no = get_mem(bg_mem + (tile_row << 5)  + tile_col);

        // Signed tile no, need to convert to offset
        if (tile_mem == TILE_SET_1_START) {
            tile_no = (tile_no & 127) - (tile_no & 128) + 128;
        }
       
        uint16_t tile_loc = tile_mem + (tile_no * 16); //Location of tile in memory
        uint8_t line_offset = (y_pos % 8) * 2; //Offset into tile of our line
        
        uint8_t byte0 = get_mem(tile_loc + line_offset);
        uint8_t byte1 = get_mem(tile_loc + line_offset + 1);
        
        // For each pixel in the line of the tile
        for (unsigned int j = 0; j < 8; j++) {
            int bit_1 = (byte1 >> (7 - j)) & 0x1;
            int bit_0 = (byte0 >> (7 - j)) & 0x1;
            int color_id = (bit_1 << 1) | bit_0;
            screen_buffer[row][i + j] =  cols[pallete[color_id]]; 
        }   
    }      

}

//Render the supplied row with background tiles
static void draw_tile_bg_row(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
   uint8_t bgp = get_mem(BGP_REF);
    int pallete[4];
    //Calculate color pallete
    pallete[0] =  bgp  & 0x3;
    pallete[1] = (bgp >> 2) & 0x3;
    pallete[2] = (bgp >> 4) & 0x3;
    pallete[3] = (bgp >> 6) & 0x3;
    
    uint8_t y_pos = row + get_mem(SCROLL_Y_REG);  
    uint8_t tile_row = y_pos >> 3; // Get row 0 - 31 of tile

    for (unsigned int i = 0; i < 160; i+= 8) {
        uint8_t x_pos = i + get_mem(SCROLL_X_REG);
        uint8_t tile_col = x_pos >> 3;
        uint8_t tile_no = get_mem(bg_mem + (tile_row << 5) + tile_col);
        // Signed tile no, need to convert to offset
        if (tile_mem == TILE_SET_1_START) {
            tile_no = (tile_no & 127) - (tile_no & 128) + 128;
        }
         
        uint16_t tile_loc = tile_mem + (tile_no * 16); //Location of tile in memory
        uint8_t line_offset = (y_pos % 8) * 2; //Offset into tile of our line

        uint8_t byte0 = get_mem(tile_loc + line_offset);
        uint8_t byte1 = get_mem(tile_loc + line_offset + 1);
        //Render entire tile row
        for (unsigned int j = 0; j < 8; j++) {
            
            int bit_1 = (byte1 >> (7 - j)) & 0x1;
            int bit_0 = (byte0 >> (7 - j)) & 0x1;
            int color_id = (bit_1 << 1) | bit_0;
            screen_buffer[row][i + j] =  cols[pallete[color_id]]; 
        }   
            
    }
}    



static void draw_tile_row() {
    uint8_t lcd_ctrl = get_mem(LCDC_REG);
    uint8_t row = get_mem(LY_REG);
    uint8_t win_y_pos = get_win_y_pos();

    uint16_t bg_mem = BG_MAP_DATA0_START; // Either bg set 0 or 1 
    uint16_t tile_mem = TILE_SET_1_START; // Either tile set 0 or 1

    // Check if using Tile set 0 or 1 
    if (lcd_ctrl & BIT_4) {
        tile_mem = TILE_SET_0_START;
    } 
    //Using Window display
    if ((lcd_ctrl & BIT_5) && (win_y_pos <= row)) {
        if (lcd_ctrl & BIT_6) { //Check which bg memory to use
            bg_mem = BG_MAP_DATA1_START; 
        }
        draw_tile_window_row(tile_mem, bg_mem, row);

    // Using BG display
    } else { 
        if (lcd_ctrl & BIT_3) { //Check which bg memory to use
            bg_mem = BG_MAP_DATA1_START;
        }
       draw_tile_bg_row(tile_mem, bg_mem, row);
    }
    
}

//Output frame
static void update_screen()
{ 
    for (int y = 0; y < 144; y++) {
        for (int x = 0; x < 160; x++) {          
            draw_pix(screen_buffer[y][x], y, x);
        }
    }    
    SDL_Flip(screen);
}

//Render the row number stored in the LY register
void draw_row() {
    uint8_t render_sprites = (get_mem(0xFF40) & BIT_1);
    uint8_t render_tiles = (get_mem(0xFF40)  & BIT_0);
    if (render_tiles) {
        draw_tile_row();
    }
    if (render_sprites) {
        draw_sprite_row();
    } 
    if (get_mem(LY_REG) == 143) {
        update_screen();
    }
}

