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


#define BIT_0 0x01              
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80

typedef enum {WHITE:0, GREY:1, DARK_GREY:2, BLACK:3} Colour;
typedef enum {TYPE0, TYPE1} TileType;

uint8_t bit_mask = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20
    , 0x40, 0x80};

typedef struct {
    Colour colour[8][8];
} Tile;


Tile get_tile(uint8_t tile_no, TileType tile_type) {

    uint16_t tile_set_mem =  
        tile_type == TYPE0 ? TILE_SET_0_START : TILE_SET_1_START;

    uint16_t tile_start_loc = tile_set_mem + (tile_no * 16);
    int i, j;
    uint8_t tile_byte1, tile_byte2;
    Tile tile;
    for (i = 0; i < 8; i++) {
        tile_byte1 = getMem(tile_start_loc);
        tile_byte2 = getMem(tile_start_lock+1);

        /*  Obtain the colour for each pixel in the row
         *  for bit n in the row, colour is calculated by
         *  the values of the nth bit of each two tile bytes */
        for (j = 0; j < 8; j++) {
            tile.colour[i][j] = (((tile_byte_1 & bit_mask[i]) >> i)
            << 1)
            | (((tile_byte_0 & bit_mask[i]) >> i);
        }

    }

    return tile;
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

Colour get_pixel(uint8_t x, uint8_t y) {
    
}
