#ifndef MAP_LIST_H
#define MAP_LIST_H
#include <stdint.h>

#include "draw.h"
#include "tile.h"
#include "tuning.h"

#define MAP_COLS  (LCD_W / TILE_SIZE)
#define MAP_ROWS  (LCD_H / TILE_SIZE)

typedef enum {
    MAP_LEVEL0,
    MAP_LEVEL1,
    MAP_LEVEL2,
    MAP_LEVEL3,
    MAP_LEVEL4,
    MAP_COUNT
} level_id_t;

typedef enum {
    LEVEL_NORMAL,
    LEVEL_BOSS,
} level_type_t;

#define LEVEL_NO_EXIT 0xFF   // use in exits[] for a portal that goes nowhere
#define LEVEL_NO_BOSS 0xFF

typedef struct {
    level_type_t type;
    uint8_t boss_id;                    // which boss, when type == LEVEL_BOSS
    uint8_t spawn_col, spawn_row;       // tile coords the player appears on
    uint8_t exits[PORTAL_COUNT];        // level_id_t reached by each portal kind
    const char *rows[MAP_ROWS];
} level_t;

extern const level_t levels[MAP_COUNT];
#endif // MAP_LIST_H
