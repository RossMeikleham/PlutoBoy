#include "cpu.h"
#include <stdio.h>
#include "mmu/memory.h"
#include "memory_layout.h"
#include <stdint.h>
#include "graphics.h"
#include <stdlib.h>
#include <unistd.h>
#include "sprite_priorities.h"
#include "bits.h"

#include "../non_core/graphics_out.h"
#include "../non_core/framerate.h"

static int screen_buffer[144][160];
static int old_buffer[144][160];

int init_gfx() {
   
    start_framerate(FPS_60); 
    int result = init_screen(GB_PIXELS_X * 2, GB_PIXELS_Y * 2, screen_buffer);
    init_sprite_prio_list();    
    
    return result;
}



static void draw_sprite_row() {
   
    // 8x16 or 8x8
    int8_t lcd_ctrl = get_mem(LCDC_REG);
    int height = lcd_ctrl & BIT_2 ? 16 : 8;
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

    Sprite_Iterator si = create_sprite_iterator();
    int sprite_no;
    int sprite_count = 0;

    /*40 Sprites, loop through from least priority to most priority
      limited to 10 a line */
    while((sprite_no = sprite_iterator_next(&si)) != -1 && sprite_count < 10)  {
        uint8_t y_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (sprite_no * 4)) - 16;
        uint8_t x_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (sprite_no * 4) + 1) - 8;
        uint8_t tile_no = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (sprite_no * 4) + 2);
        uint8_t attributes = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (sprite_no * 4) + 3);

        if (height == 16) {
            tile_no &= ~0x1;
        }                           
        
        //If sprite doesn't intersect current line, no need to draw
        if (y_pos > row || row >= y_pos + height ||  x_pos >= 160) {
            continue;
        } 
        
        sprite_count++;

        int x_flip = attributes & BIT_5;
        int y_flip = attributes & BIT_6;

        uint16_t tile_loc = TILE_SET_0_START + (tile_no * 16);

        // Obtain row of sprite to draw, if sprite is flipped
        // need to obtain row relative to bottom of sprite
        uint8_t line = (!y_flip) ? row - y_pos  : height + y_pos - row;
        uint16_t line_offset = 2 * line;
        uint8_t high_byte = get_mem(tile_loc + line_offset);
        uint8_t low_byte =  get_mem(tile_loc + line_offset + 1);
        int pal_no = (attributes & BIT_4) ? 1 : 0;
        
        // Draw all pixels in current line of sprite
        for (int x = 0; x < 8; x++) {
            // Not on screen
            if (x_pos + x >= 160) {
                continue;
            }
            int bit_pos = (x_flip) ? x : 7 - x;
            int bit_0 = (low_byte >> bit_pos) & 0x1;
            int bit_1 = (high_byte >> bit_pos) & 0x1;
            uint8_t color_id = (bit_0 << 1) | bit_1;
            int priority  = !(attributes & 0x80);
            
            // If priority bit not set but background is transparent and
            // current pixel isn't transparent draw. Otherwise if priority set
            // as long as pixel isn't transparent, draw it
            uint8_t final_color_id = palletes[pal_no][color_id]; 
            if (!priority) {
                if (old_buffer[row][x_pos + x] == 0 && color_id != 0) {
                   screen_buffer[row][x_pos + x] = final_color_id;
                   old_buffer[row][x_pos + x] = color_id;
                }               
            } else  {
                if (color_id != 0) {
                   screen_buffer[row][x_pos + x] = final_color_id;
                   old_buffer[row][x_pos + x] = color_id;
                }
            }     
             
        }
    }
}

static void draw_single_win_tile(uint16_t tile_mem, uint16_t bg_mem, uint8_t row,
    uint8_t start_x, uint8_t x_pos, uint8_t y_pos, uint8_t start_pix, uint8_t end_pix, int *pallete) {

    uint8_t tile_row = y_pos >> 3;
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
    for (unsigned int j = start_pix; j < end_pix; j++) {
        //Not on screen, don't draw 
        if (start_x + j - start_pix < x_pos) {
        //    break;
        } 
        int bit_1 = (byte1 >> (7 - j)) & 0x1;
        int bit_0 = (byte0 >> (7 - j)) & 0x1;
        int color_id = (bit_1 << 1) | bit_0;
        screen_buffer[row][start_x + j - start_pix] =  pallete[color_id]; 
        old_buffer[row][start_x + j - start_pix] = color_id;
    }
}
static void draw_single_bg_tile(uint16_t tile_mem, uint16_t bg_mem, uint8_t row,
    uint8_t start_x, uint8_t x_pos, uint8_t y_pos, uint8_t start_pix, uint8_t end_pix, int *pallete) {

    uint8_t tile_row = y_pos >> 3;
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
    for (unsigned int j = start_pix; j < end_pix; j++) {
        
        int bit_1 = (byte1 >> (7 - j)) & 0x1;
        int bit_0 = (byte0 >> (7 - j)) & 0x1;
        int color_id = (bit_1 << 1) | bit_0;
        screen_buffer[row][start_x + j - start_pix] =  pallete[color_id]; 
        old_buffer[row][start_x + j - start_pix] = color_id;
    }
}



//Render the supplied row with window tiles
static void draw_tile_window_row(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
   
    uint8_t bgp = get_mem(BGP_REF);
    int pallete[4];
    //Calculate color pallete
    pallete[0] =  bgp  & 0x3;
    pallete[1] = (bgp >> 2) & 0x3;
    pallete[2] = (bgp >> 4) & 0x3;
    pallete[3] = (bgp >> 6) & 0x3;

    uint8_t win_y = get_mem(WY_REG);  
    uint8_t y_pos = row - win_y; // Get line 0 - 255 being drawn    
    //uint16_t tile_row = (y_pos / 8); // Get row 0 - 31 of tile
    uint8_t win_x = get_mem(WX_REG) - 7;
    //negative win x
   // if (win_x > 248) {
   //     return;
   // } 
    uint8_t tile_skew = win_x % 8;
    
    if (tile_skew) {
        draw_single_win_tile(tile_mem, bg_mem, row, 0, -win_x, y_pos, tile_skew, 8, pallete);
        draw_single_win_tile(tile_mem, bg_mem, row, 160 - tile_skew, 160 -tile_skew - win_x,  y_pos, 0, tile_skew, pallete); 
    }
    // Render all full tiles
    uint8_t start = tile_skew ? 8 - tile_skew : 0;
    uint8_t end = tile_skew ? 160 - tile_skew : 160;
    for (unsigned int i = start; i < end; i+= 8) {
        draw_single_win_tile(tile_mem, bg_mem, row, i, i - win_x,  y_pos,0, 8, pallete);
    }
}    




static void draw_tile_bg_row(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
   uint8_t bgp = get_mem(BGP_REF);
    int pallete[4];
    //Calculate color pallete
    pallete[0] =  bgp  & 0x3;
    pallete[1] = (bgp >> 2) & 0x3;
    pallete[2] = (bgp >> 4) & 0x3;
    pallete[3] = (bgp >> 6) & 0x3;
    
    uint8_t y_pos = row + get_mem(SCROLL_Y_REG); 
    uint8_t scroll_x = get_mem(SCROLL_X_REG); 

    // If scroll x is not divisible by 8 then start and end
    // of row will have rendered parts of tiles
 
    uint8_t tile_skew = get_mem(SCROLL_X_REG) % 8;
    if (tile_skew != 0) {
       draw_single_bg_tile(tile_mem, bg_mem, row, 0, scroll_x,  y_pos, tile_skew,  8, pallete);
       draw_single_bg_tile(tile_mem, bg_mem, row, 160 - tile_skew, 160 - tile_skew + scroll_x, y_pos, 0, tile_skew, pallete); 
    } 
    // Render all full tiles
    uint8_t start = tile_skew ? 8 - tile_skew : 0;
    uint8_t end = tile_skew ? 160 - tile_skew -1 : 160;
    for (unsigned int i = start; i < end; i+= 8) {
      draw_single_bg_tile(tile_mem, bg_mem, row, i, i + scroll_x,  y_pos, 0, 8, pallete);
    }
}    
        

      

static void draw_tile_row() {
    uint8_t lcd_ctrl = get_mem(LCDC_REG);
    uint8_t row = get_mem(LY_REG);
    uint8_t win_y_pos = get_mem(WY_REG);

    uint16_t tile_mem; // Either tile set 0 or 1

    // Check if using Tile set 0 or 1 
    tile_mem = lcd_ctrl & BIT_4 ? TILE_SET_0_START : TILE_SET_1_START;
    
    //Draw background    
    uint16_t bg_mem = lcd_ctrl & BIT_3 ? BG_MAP_DATA1_START : BG_MAP_DATA0_START;
    draw_tile_bg_row(tile_mem, bg_mem, row);

    //Draw Window display if it's on
    if ((lcd_ctrl & BIT_5) && (win_y_pos <= row)) {
        uint16_t win_bg_mem = lcd_ctrl & BIT_6 ? BG_MAP_DATA1_START :BG_MAP_DATA0_START;
        draw_tile_window_row(tile_mem, win_bg_mem, row);
    }    
}


void output_screen() {
    draw_screen();
    adjust_to_framerate();

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
}

