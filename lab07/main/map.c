#include "map.h"
#include "draw.h"
#include "config.h"

static uint8_t tiles[MAP_ROWS][MAP_COLS];

void map_load(map_state_t state) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            tiles[r][c] = levels[state][r][c];
}

void map_draw(void) {
    bool filled;
    color_t color;
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (tiles[r][c] == '#' || tiles[r][c] == '%') {
                color = WHITE;
                filled = true;
            } else if (tiles[r][c] == 'X') {
                color = GREEN;
                filled = true;
            } else if (tiles[r][c] == '>') {
                color = PURPLE;
                filled = true;
            } else if (tiles[r][c] == '<') {
                color = BLUE;
                filled = true;
            } else if (tiles[r][c] == '*') {
                color = YELLOW;
                filled = true;
            } else if (tiles[r][c] == '/') {
                color = CYAN;
                filled = true;
            } else {
                filled = false;
            }
            if (filled)
                better_rect_fill(c * TILE + TILE / 2, r * TILE + TILE / 2, TILE, TILE, color);
        }
    }
}

uint8_t map_solid_at(int32_t px, int32_t py) {
    if (px < 0 || px >= LCD_W) return '%';
    if (py < 0 || py >= LCD_H) return '%';
    return tiles[py / TILE][px / TILE];
}