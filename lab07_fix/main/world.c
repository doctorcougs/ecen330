#include "world.h"

#include "tuning.h"

void world_init(world_t *w, level_id_t first_level) {
    player_init(&w->player);
    world_set_level(w, first_level);
}

void world_set_level(world_t *w, level_id_t id) {
    w->level_id = id;
    w->level = &levels[id];
    map_load(w->level);
    player_spawn(&w->player,
                 w->level->spawn_col * TILE_SIZE + TILE_SIZE / 2,
                 w->level->spawn_row * TILE_SIZE + TILE_SIZE / 2);
}

bool world_take_portal(world_t *w, portal_kind_t kind) {
    uint8_t target = w->level->exits[kind];
    if (target == LEVEL_NO_EXIT || target >= MAP_COUNT) return false;
    world_set_level(w, (level_id_t)target);
    return true;
}
