#ifndef MAP_H
#define MAP_H
#include <stdint.h>
#include <stdbool.h>
#include "../map/map_list.h"

void map_load(map_state_t state);    // copy the level into the active grid and draw it
void map_draw(void);           // redraw all tiles (use after a fillScreen)
uint8_t map_solid_at(int32_t px, int32_t py);   // pixel coords -> is there a solid tile?
#endif // MAP_H