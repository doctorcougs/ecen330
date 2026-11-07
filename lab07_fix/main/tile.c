#include "tile.h"

static const tile_info_t table[256] = {
    [(uint8_t)TILE_SOLID]  = {TILE_F_SOLID | TILE_F_GRIP | TILE_F_VISIBLE, 0, WHITE},
    [(uint8_t)TILE_BOUNDS] = {TILE_F_SOLID | TILE_F_VISIBLE, 0, WHITE},
    [(uint8_t)TILE_HAZARD] = {TILE_F_SOLID | TILE_F_HAZARD | TILE_F_VISIBLE, 0, GREEN},
    [(uint8_t)TILE_NEXT]   = {TILE_F_PORTAL | TILE_F_VISIBLE, PORTAL_NEXT,   PURPLE},
    [(uint8_t)TILE_PREV]   = {TILE_F_PORTAL | TILE_F_VISIBLE, PORTAL_PREV,   BLUE},
    [(uint8_t)TILE_DOUBLE] = {TILE_F_PORTAL | TILE_F_VISIBLE, PORTAL_DOUBLE, YELLOW},
    [(uint8_t)TILE_HALF]   = {TILE_F_PORTAL | TILE_F_VISIBLE, PORTAL_HALF,   CYAN},
};

const tile_info_t *tile_info(char ch) {
    return &table[(uint8_t)ch];
}
