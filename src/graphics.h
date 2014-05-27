#ifndef GRAPHICS_H
#define GRAPHICS_H


#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

typedef enum {WHITE = 0, GREY = 1, DARK_GREY = 2, BLACK = 3} Colour;
typedef enum {TYPE0 = 0, TYPE1 = 1} TileType;

typedef struct {
    Colour colour[8][8];
} Tile;


typedef struct  {

    TileType type;
    uint8_t tiles[32][32]; /* Stores tile numbers */

} Background;


int init_gfx();

void draw_row(uint8_t row);
void draw_tile_0(uint8_t tile_no, uint8_t x, uint8_t y);
void draw_tile_1(int8_t tile_no, uint8_t x, uint8_t y);
void draw_background_0(void);
void draw_background_1(void);

#endif /* GRAPHICS_H */
