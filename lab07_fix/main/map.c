#include "map.h"
#include "draw.h"
#include "config.h"
#include "tile.h"
#include "tuning.h"

static char tiles[MAP_ROWS][MAP_COLS];

void map_load(const level_t *level) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            tiles[r][c] = level->rows[r][c];
}

static void draw_tile(int r, int c) {
    const tile_info_t *t = tile_info(tiles[r][c]);
    if (t->flags & TILE_F_VISIBLE)
        lcd_fillRect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE, t->color);
}

void map_draw(void) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            draw_tile(r, c);
}

void map_draw_region(int32_t x, int32_t y, int32_t w, int32_t h) {
    int32_t c0 = x / TILE_SIZE, c1 = (x + w - 1) / TILE_SIZE;
    int32_t r0 = y / TILE_SIZE, r1 = (y + h - 1) / TILE_SIZE;
    if (x < 0) c0 = 0;
    if (y < 0) r0 = 0;
    if (c1 >= MAP_COLS) c1 = MAP_COLS - 1;
    if (r1 >= MAP_ROWS) r1 = MAP_ROWS - 1;
    for (int32_t r = r0; r <= r1; r++) {
        for (int32_t c = c0; c <= c1; c++) {
            lcd_fillRect(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE, CONFIG_COLOR_BACKGROUND);
            draw_tile(r, c);
        }
    }
}

char map_tile_at(int32_t px, int32_t py) {
    if (px < 0 || px >= LCD_W) return TILE_BOUNDS;
    if (py < 0 || py >= LCD_H) return TILE_BOUNDS;
    return tiles[py / TILE_SIZE][px / TILE_SIZE];
}
