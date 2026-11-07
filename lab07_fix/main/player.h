#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "entity.h"
#include "input.h"

typedef enum {
    PLAYER_IDLE,
    PLAYER_MOVEMENT,
    PLAYER_DEAD,
} player_state_t;

typedef struct {
    entity_t body;           // position, velocity, health, i-frames (shared with the boss)
    player_state_t state;
} player_t;

void player_init(player_t *player);

// Put the player at a pixel position (center) with zero velocity.
void player_spawn(player_t *player, float x, float y);

// One update step. Does NOT draw. Returns the entity events from this tick's
// movement so the caller (game/world) can react to portals.
entity_events_t player_tick(player_t *player, const input_t *input);

void player_draw(player_t *player);
void player_erase(player_t *player);

static inline bool player_is_dead(const player_t *player) { return !entity_alive(&player->body); }

#endif // PLAYER_H
