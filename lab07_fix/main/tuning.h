#ifndef TUNING_H
#define TUNING_H

// Gameplay numbers.
//
// NOTE: every "PER_TICK" value assumes the fixed update period
// CONFIG_GAME_TIMER_PERIOD (40 ms). If you change that period, the feel of
// the whole game changes with it; re-tune these (or add a dt multiplier).

#define TILE_SIZE 10

// Player hitbox (used for collision) and art size (used for drawing) are
// separate so the sprite can differ from the hitbox later.
#define PLAYER_HITBOX_W 10
#define PLAYER_HITBOX_H 10
#define PLAYER_DRAW_W   10
#define PLAYER_DRAW_H   10

#define PLAYER_STARTING_IFRAMES_CAP 10   // ticks of invulnerability after a hit
#define PLAYER_STARTING_HEALTH 4

#define CHARACTER_ACCEL_PER_TICK 0.4f
#define CHARACTER_JUMP_PER_TICK  5.f
#define GRAVITY_ACCEL_PER_TICK   0.5f
#define MAX_CHAR_SPEED           5.0f
#define FRICTION                 0.85f

#define JOYSTICK_MOVE_SENSE 500

// entity_move() splits movement into sub-steps no longer than this, so fast
// things (projectiles, dashes) can't tunnel through a wall.
#define ENTITY_MAX_STEP (TILE_SIZE / 2.0f)

// Normal characters should never need sub-stepping; this catches a bad tune.
_Static_assert((int)MAX_CHAR_SPEED < TILE_SIZE,
               "MAX_CHAR_SPEED must stay below TILE_SIZE");

#endif // TUNING_H
