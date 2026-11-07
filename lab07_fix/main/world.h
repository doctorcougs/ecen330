#ifndef WORLD_H
#define WORLD_H
#include <stdbool.h>

#include "map.h"
#include "player.h"

// Everything that makes up the running game. game.c owns one of these and
// passes it down, so "reset" is a single world_init() call. Boss, projectiles
// etc. will live here too.
typedef struct {
    player_t player;
    level_id_t level_id;
    const level_t *level;
} world_t;

// Fresh player in the given level.
void world_init(world_t *w, level_id_t first_level);

// Switch levels: loads the tiles and puts the player on the level's spawn tile.
void world_set_level(world_t *w, level_id_t id);

// Try to leave through a portal of this kind. Returns true (and switches
// level) if the current level has an exit for it. This is where boss levels
// can later refuse to open until the boss is dead.
bool world_take_portal(world_t *w, portal_kind_t kind);

#endif // WORLD_H
