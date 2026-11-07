#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "tile.h"

// Everything that moves and collides with the map (player, boss, projectiles)
// embeds one of these and uses the entity_* functions. Anything specific to
// one kind of thing (input, AI, level progress) stays in that kind's struct.
typedef struct {
    float x, y;            // CENTER of the hitbox, in pixels
    float dx, dy;          // velocity, pixels per tick
    uint8_t w, h;          // hitbox size (collision)
    uint8_t draw_w, draw_h;// art size (drawing), may differ from the hitbox
    float max_speed;       // per-axis speed clamp

    int8_t health;
    uint16_t iframes_cap;      // ticks of invulnerability after taking damage
    uint16_t iframes_counter;  // ticks left, counts down every tick

    bool can_jump;         // set by touching a "grip" tile, cleared by the owner when it jumps

    // where it was last drawn, so it can be erased without a full redraw
    int16_t drawn_x, drawn_y;
    bool drawn;
} entity_t;

// Events reported by entity_move(). The entity never reacts to these itself;
// the caller (player, boss, ...) decides what each one means.
typedef uint16_t entity_events_t;
#define ENT_EVT_HAZARD       (1u << 0)   // pushed against a hazard tile
#define ENT_EVT_LANDED       (1u << 1)   // stopped by something while falling
#define ENT_EVT_PORTAL(kind) (1u << (2 + (kind)))   // overlapping a portal of this kind
#define ENT_EVT_PORTAL_MASK  (((1u << PORTAL_COUNT) - 1u) << 2)

void entity_init(entity_t *e, uint8_t hit_w, uint8_t hit_h, uint8_t draw_w, uint8_t draw_h,
                 int8_t health, uint16_t iframes_cap);

// Put the entity at (x, y) with zero velocity (e.g. on level entry).
void entity_place(entity_t *e, float x, float y);

// Apply one tick of acceleration: velocity += accel, friction when there is no
// horizontal push, then clamp to max_speed.
void entity_integrate(entity_t *e, float ddx, float ddy);

// Move by the current velocity, resolving collisions against the map.
entity_events_t entity_move(entity_t *e);

// Count invulnerability down. Call once per tick.
void entity_tick_timers(entity_t *e);

// Take one point of damage unless invulnerable. Returns true if it landed.
bool entity_damage(entity_t *e);

static inline bool entity_alive(const entity_t *e) { return e->health > 0; }

// Drawing is separate from updating, see game.c for the order.
void entity_draw(entity_t *e, color_t color);
void entity_erase(entity_t *e);        // restores the map under the last drawn spot
void entity_forget_draw(entity_t *e);  // after a full-screen redraw

#endif // ENTITY_H
