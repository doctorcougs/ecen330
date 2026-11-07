#ifndef TILE_H
#define TILE_H
#include <stdint.h>

#include "draw.h"

// One place that says what each map character means. Add a new tile type by
// adding a #define and one row in tile.c.

#define TILE_EMPTY   '.'
#define TILE_SOLID   '#'   // wall/floor you can jump off of
#define TILE_BOUNDS  '%'   // returned for off-screen, solid but no jump reset
#define TILE_HAZARD  'X'   // solid, damages
#define TILE_NEXT    '>'   // portals: where they lead is in the level's exits[]
#define TILE_PREV    '<'
#define TILE_DOUBLE  '*'
#define TILE_HALF    '/'

#define TILE_F_SOLID   0x01  // stops movement
#define TILE_F_GRIP    0x02  // touching it (side/floor) re-enables jumping
#define TILE_F_HAZARD  0x04
#define TILE_F_PORTAL  0x08
#define TILE_F_VISIBLE 0x10  // drawn as a filled tile of .color

typedef enum {
    PORTAL_NEXT,
    PORTAL_PREV,
    PORTAL_DOUBLE,
    PORTAL_HALF,
    PORTAL_COUNT,
} portal_kind_t;

typedef struct {
    uint8_t flags;
    uint8_t portal;   // portal_kind_t, only meaningful with TILE_F_PORTAL
    color_t color;
} tile_info_t;

// Never returns NULL; unknown characters are empty and invisible.
const tile_info_t *tile_info(char ch);

#endif // TILE_H
