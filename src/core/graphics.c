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


// Obtains color-id (0 - 3) of pixel number in the given 
// line. pixel_no must be between 0 and 7
int get_color_id(uint8_t byte0, uint8_t byte1, int pixel_no, uint16_t pallete_mem) {
   
   //TODO fix 
    pixel_no = 7 - pixel_no;
    int bit_0 = !!(byte0 & (0x80 >> pixel_no));
    int bit_1 = !!(byte1 & (0x80 >> pixel_no));
    
    // Pass through internal pallete to get new bit color
    uint8_t pallete = get_mem(pallete_mem);

    if (bit_0) {
        if (bit_1) {
            return pallete >> 6;
        } else {
            return (pallete & 0x7F) >> 4;
        }
    } else {
        if (bit_1) {
            return (pallete & 0x0F) >> 2;
        } else {
            return pallete & 0x3;
        }
    }
    /*
    // Work out which bits in internal pallete to use
    int pallete_bit_no_0 = (4 * bit_0) + (2 * bit_1) + 1;
    int pallete_bit_no_1 = pallete_bit_no_0 - 1;


    int pallete_bit_0 = !!(pallete & (1 << pallete_bit_no_0));
    int pallete_bit_1 = !!(pallete & (1 << pallete_bit_no_1));
    
    return (pallete_bit_0 << 1) | (pallete_bit_1);
    return (bit_0 << 1) | bit_1;*/
}


void draw_sprite_row(uint8_t row) {
    
    uint8_t lcd_ctrl = get_mem(LCDC_REG);
    int extended_sprite = !!(lcd_ctrl & BIT_2);

    // 40 Sprites
    for (int i = 0; i < 40; i++) {
        uint8_t y_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4)) - 16;
        uint8_t x_pos = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 1) - 8;
        uint8_t tile_no = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 2);
        uint8_t attributes = get_mem(SPRITE_ATTRIBUTE_TABLE_START + (i * 4) + 3);
        int y_flip = attributes & BIT_6;
        int x_flip = attributes & BIT_5;
        uint8_t height = 8 * (1 + extended_sprite); // 8x16 if extended
        
        printf("y pos: %u, x pos :%u tile_no : %u\n",y_pos,x_pos,tile_no);
        // Part of sprite is on current row being drawn
        if (y_pos <= row && y_pos + height >= row) {
                        
            uint16_t tile_loc = TILE_SET_0_START + (tile_no * 16);
            // Obtain row of sprite to draw, if sprite is flipped
            // need to obtain row relative to bottom of sprite
            uint8_t line = (!y_flip) ? row - y_pos : height + y_pos  - row;
            uint16_t line_offset = 2 * line;
 
            uint8_t byte0 = get_mem(tile_loc + line_offset);
            uint8_t byte1 = get_mem(tile_loc + line_offset + 1);

            uint16_t pallete_addr = (attributes & BIT_4) ? OBP1_REG : OBP0_REG;
            // Draw all pixels on row
            for (unsigned int i = 0; i < 8; i++) {
                // Read bits in reverse if x flipped
                int bit_no = (!x_flip) ? i : 7 - i;
                int color_id = get_color_id(byte0, byte1, bit_no, pallete_addr);

                // Draw if not transparent pixel
                if (color_id != 0) {
                    printf("color id %d x: %u  y:%u\n",color_id, x_pos + i, y_pos);
                    draw_pix(color_id, x_pos + i, y_pos);
                }

            }
        }

    }
    
    printf("SPRITES DRAWN\n");
    update_screen();
}


void draw_tile_window_line(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
    
    uint8_t scroll_x = get_scroll_x();
    uint8_t y_pos = row - get_win_y_pos(); // Get line 0 - 255 being drawn    
    uint16_t tile_row = (y_pos / 8); // Get row 0 - 31 of tile
    uint8_t win_x_pos = get_win_x_pos() - 7;
    
    // 160 pixel row, 20 tiles, 8 pixel row per tile
    for (unsigned int i = 0; i < 20; i++) {

        uint16_t tile_col = i + scroll_x/8; //Get column 0 - 31 of tile
        if (tile_col >= (win_x_pos/8)) {
            tile_col -= (win_x_pos/8);
        }
       
        uint8_t tile_no = get_mem(bg_mem + (tile_row * 32) + tile_col);
        
        // Signed tile no, need to convert to offset
        if (tile_mem == TILE_SET_1_START) {
            tile_no = (tile_no & 127) - (tile_no & 127) + 128;
        }
       
        uint16_t tile_loc = tile_mem + (tile_no * 16); //Location of tile in memory
        uint8_t line_offset = (y_pos % 8) * 2; //Offset into tile of our line
        
        uint8_t byte0 = get_mem(tile_loc + line_offset);
        uint8_t byte1 = get_mem(tile_loc + line_offset + 1);
        
        // For each pixel in the line of the tile
        for (unsigned int j = 0; j < 8; j++) {
            int color_id = get_color_id(byte0, byte1, j, BGP_REF);

            int x_pos = i * 8 + j + scroll_x;
            if (x_pos >= win_x_pos) {
                x_pos -= win_x_pos;
            }
            draw_pix(color_id, x_pos, y_pos);
        }   
    printf("Window drawn\n");    
    }     

}


void draw_tile_bg_line(uint16_t tile_mem, uint16_t bg_mem, uint8_t row) {
    
    uint8_t scroll_x = get_scroll_x();
    uint8_t y_pos = get_scroll_y() + row; // Get line 0 - 255 being drawn    
    uint16_t tile_row = (y_pos / 8); // Get row 0 - 31 of tile
    // 160 pixel row, 20 tiles, 8 pixel row per tile
    for (unsigned int i = 0; i < 20; i++) {
        uint16_t tile_col = i + scroll_x/8; //Get column 0 - 31 of tile
        uint8_t tile_no = get_mem(bg_mem + (tile_row * 32) + tile_col);
        
        // Signed tile no, need to convert to offset
        if (tile_mem == TILE_SET_1_START) {
            tile_no = (tile_no & 127) - (tile_no & 127) + 128;
        }
        //printf("tile no %u\n",tile_no);
        uint16_t tile_loc = tile_mem + (tile_no * 16); //Location of tile in memory
        uint8_t line_offset = (y_pos % 8) * 2; //Offset into tile of our line

        uint8_t byte0 = get_mem(tile_loc + line_offset);
        uint8_t byte1 = get_mem(tile_loc + line_offset + 1);
        
        // For each pixel in the line of the tile
        for (unsigned int j = 0; j < 8; j++) {
            int color_id = get_color_id(byte0, byte1, j, BGP_REF); 
            draw_pix(color_id, i*8 + j + scroll_x, y_pos);
        }   
        
    }
    
}

void draw_tile_row(uint8_t row) {
    uint8_t lcd_ctrl = get_mem(LCDC_REG);
    
    uint8_t win_y_pos = get_win_y_pos();

    uint16_t bg_mem = BG_MAP_DATA0_START; // Either bg set 0 or 1 
    uint16_t tile_mem = TILE_SET_1_START; // Either tile set 0 or 1

    // Check if using Tile set 0 or 1 
    if ((lcd_ctrl & BIT_4)) {
        tile_mem = TILE_SET_0_START;
    } 
    //Using Window display
    if ((lcd_ctrl & BIT_5) && (win_y_pos <= row)) {
        if (lcd_ctrl & BIT_6) {
            bg_mem = BG_MAP_DATA1_START;
        }
        draw_tile_window_line(tile_mem, bg_mem, row);

    // Using BG display
    } else {
        if (lcd_ctrl & BIT_3) {
            bg_mem = BG_MAP_DATA1_START;
        }
        draw_tile_bg_line(tile_mem, bg_mem, row);
    }
    
    update_screen();
}





void draw_sp_row(uint8_t row) {

    uint8_t render_sprites = (get_mem(LCDC_REG) & BIT_0);
    uint8_t render_tiles = (get_mem(LCDC_REG) & BIT_1);
    if (render_tiles) {
        
        draw_tile_row(row);
    } 
    if (render_sprites) {

        draw_sprite_row(row);
    }
    update_screen();

}


/*  void draw_row() {
    draw_sp_row(get_mem(LY_REG));
}*/

 
void sprites_notmine(uint8_t lcdControl) {
   int use8x16 = 0 ;
   if (lcdControl & BIT_2)
     use8x16 = 1 ;

   for (int sprite = 0 ; sprite < 40; sprite++)
   {
     // sprite occupies 4 bytes in the sprite attributes table
     uint8_t index = sprite*4 ;
     uint8_t yPos = get_mem(0xFE00+index) - 16;
     uint8_t xPos = get_mem(0xFE00+index+1)-8;
     uint8_t tileLocation = get_mem(0xFE00+index+2) ;
     uint8_t attributes = get_mem(0xFE00+index+3) ;

     int yFlip = attributes & BIT_6 ;
     int xFlip = attributes & BIT_5 ;

     int scanline = get_mem(0xFF44);

     int ysize = 8;
     if (use8x16)
       ysize = 16;

     // does this sprite intercept with the scanline?
     if ((scanline >= yPos) && (scanline < (yPos+ysize)))
     {
       int line = scanline - yPos ;

       // read the sprite in backwards in the y axis
       if (yFlip)
       {
         line -= ysize ;
         line *= -1 ;
       }

       line *= 2; // same as for tiles
       uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line ;
       uint8_t data1 = get_mem( dataAddress ) ;
       uint8_t data2 = get_mem( dataAddress +1 ) ;

       // its easier to read in from right to left as pixel 0 is
       // bit 7 in the colour data, pixel 1 is bit 6 etc...
       for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
       {
         int colourbit = tilePixel ;
         // read the sprite in backwards for the x axis
         if (xFlip)
         {
           colourbit -= 7 ;
           colourbit *= -1 ;
         }

         // the rest is the same as for tiles
         uint16_t colourAddress = (attributes & BIT_4)?0xFF48:0xFF49 ;
         
         
         int col= get_color_id(data2, data1, colourbit, colourAddress ) ;

         // white is transparent for sprites.
         if (col == 0)
           continue ;

         int xPix = 0 - tilePixel ;
         xPix += 7 ;

         int pixel = xPos+xPix ;

         // sanity check
         if ((scanline<0)||(scanline>143)||(pixel<0)||(pixel>159))
         {
           continue ;
         }
        
         screen_buffer[scanline][pixel] = cols[col];
       }
     }
   }
} 


void draw_notmytiles(uint8_t lcdControl) {

   uint16_t tileData = 0 ;
   uint16_t backgroundMemory =0 ;
   int unsig = 1 ;

   // where to draw the visual area and the window
   uint8_t scrollY = get_mem(0xFF42) ;
   uint8_t scrollX = get_mem(0xFF43) ;
   uint8_t windowY = get_mem(0xFF4A) ;
   uint8_t windowX = get_mem(0xFF4B) - 7;
   int usingWindow = 0 ;

   // is the window enabled?
   if (lcdControl & BIT_5)
   {
     // is the current scanline we're drawing
     // within the windows Y pos?,
     if (windowY <= get_mem(0xFF44))
       usingWindow = 1 ;
   }

   // which tile data are we using?
   if (lcdControl & BIT_4)
   {
     tileData = 0x8000 ;
   }
   else
   {
     // IMPORTANT: This memory region uses signed
     // bytes as tile identifiers
     tileData = 0x8800 ;
     unsig= 0 ;
   }
   // which background mem?
   if (!usingWindow)
   {
     if (lcdControl & BIT_3)
       backgroundMemory = 0x9C00 ;
     else
       backgroundMemory = 0x9800 ;
   }
   else
   {
     // which window memory?
     if (lcdControl & BIT_6)
       backgroundMemory = 0x9C00 ;
     else
       backgroundMemory = 0x9800 ;
   }

   uint8_t yPos = 0 ; 
   // yPos is used to calculate which of 32 vertical tiles the
   // current scanline is drawing
   if (!usingWindow)
     yPos = scrollY + get_mem(0xFF44) ;
   else
     yPos = get_mem(0xFF44) - windowY;

   // which of the 8 vertical pixels of the current
   // tile is the scanline on?
   uint16_t tileRow = (((uint8_t)(yPos/8))*32) ;

   // time to start drawing the 160 horizontal pixels
   // for this scanline
   for (int pixel = 0 ; pixel < 160; pixel++)
   {
     uint8_t xPos = pixel+scrollX ;

     // translate the current x pos to window space if necessary
     if (usingWindow)
     {
       if (pixel >= windowX)
         {
           xPos = pixel - windowX ;
         }
     }

     // which of the 32 horizontal tiles does this xPos fall within?
     uint16_t tileCol = (xPos/8) ;
     int16_t tileNum ;

     // get the tile identity number. Remember it can be signed
     // or unsigned
     uint16_t tileAddrss = backgroundMemory+tileRow+tileCol;
     if(unsig)
       tileNum =(uint8_t)get_mem(tileAddrss);
     else
       tileNum =(int8_t)get_mem(tileAddrss );

     // deduce where this tile identifier is in memory. Remember i
     // shown this algorithm earlier
     uint16_t tileLocation = tileData ;
     
     if (unsig) {
       tileLocation += (tileNum * 16) ;
     } else
       tileLocation += ((tileNum+128) *16) ;

     // find the correct vertical line we're on of the
     // tile to get the tile data
     //from in memory
     uint8_t line = yPos % 8 ;
     line *= 2; // each vertical line takes up two bytes of memory
    
     uint8_t data1 = get_mem(tileLocation + line) ;
     uint8_t data2 = get_mem(tileLocation + line + 1) ;

     // pixel 0 in the tile is it 7 of data 1 and data2.
     // Pixel 1 is bit 6 etc..
     int colourBit = xPos % 8 ;
     colourBit -= 7 ;
     colourBit *= -1 ;

     // now we have the colour id get the actual
     // colour from palette 0xFF47
     int col_id = get_color_id(data2, data1, colourBit, 0xFF47) ;
     
     int finaly = get_mem(0xFF44) ;
     // safety check to make sure what im about
     // to set is int the 160x144 bounds
     if ((finaly<0)||(finaly>143)||(pixel<0)||(pixel>159))
     {
       continue ;
     }
  
     screen_buffer[finaly][pixel] = cols[col_id];
    }
}

void update_entire_screen()
{
    int width_inc = Screen_Width/SCREEN_WIDTH;
    int height_inc = Screen_Height/SCREEN_HEIGHT;
    
    for (unsigned y = 0; y < 144; y++) {
        for (unsigned x = 0; x < 160; x++) {
            
            SDL_Rect rect = {x * width_inc, y * height_inc, width_inc, height_inc};
            SDL_FillRect(screen, &rect, screen_buffer[y][x]);
        }
    }
    SDL_Flip(screen);
}

void draw_row() {
    uint8_t render_sprites = (get_mem(0xFF40) & BIT_1);
    uint8_t render_tiles = (get_mem(0xFF40)  & BIT_0);
    uint8_t lcdControl = get_mem(LCDC_REG);
    if (render_tiles) {
        draw_notmytiles(lcdControl);
    }
    if (render_sprites) {
        sprites_notmine(lcdControl);
        
    } 
 
    if (get_mem(LY_REG) == 143) {
        update_entire_screen();
    }
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


