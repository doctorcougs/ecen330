#ifndef MAP_H
#define MAP_H
#include <stdint.h>
#include <stdbool.h>
#include "../map/map_list.h"

void map_load(const level_t *level);   // copy the level's tiles into the active grid
void map_draw(void);                   // draw every tile (after a fillScreen)
// Repaint the given pixel rectangle (top-left x, y) back to background + tiles.
void map_draw_region(int32_t x, int32_t y, int32_t w, int32_t h);
// Tile character at a pixel; TILE_BOUNDS outside the screen.
char map_tile_at(int32_t px, int32_t py);
#endif // MAP_H
