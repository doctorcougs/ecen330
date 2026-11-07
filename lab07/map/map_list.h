#ifndef MAP_LIST_H
#define MAP_LIST_H
#include "draw.h"


#define TILE      10
#define MAP_COLS  (LCD_W / TILE)
#define MAP_ROWS  (LCD_H / TILE)

typedef enum {
    MAP_LEVEL0,
    MAP_LEVEL1,
    MAP_LEVEL2,
    MAP_LEVEL3,
    MAP_LEVEL4,
    MAP_COUNT
} map_state_t;


extern const char *levels[MAP_COUNT][MAP_ROWS];
#endif // MAP_LIST_H
