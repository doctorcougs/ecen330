#include "entity.h"

#include <math.h>

#include "config.h"
#include "map.h"
#include "tile.h"
#include "tuning.h"

static inline int32_t floor_div(int32_t a, int32_t b) {
    int32_t q = a / b;
    if ((a % b != 0) && ((a < 0) != (b < 0))) q--;
    return q;
}

// OR together the tile flags along a vertical line (column px, rows y0..y1)
// or a horizontal line (row py, columns x0..x1), sampling at least once per
// tile so entities taller/wider than a tile can't clip a wall in the middle.
static uint8_t scan_column(int32_t px, int32_t y0, int32_t y1) {
    uint8_t f = 0;
    for (int32_t y = y0;; y += TILE_SIZE) {
        if (y > y1) y = y1;
        f |= tile_info(map_tile_at(px, y))->flags;
        if (y == y1) break;
    }
    return f;
}

static uint8_t scan_row(int32_t py, int32_t x0, int32_t x1) {
    uint8_t f = 0;
    for (int32_t x = x0;; x += TILE_SIZE) {
        if (x > x1) x = x1;
        f |= tile_info(map_tile_at(x, py))->flags;
        if (x == x1) break;
    }
    return f;
}

typedef struct { int32_t left, right, top, bottom; } box_t;

static box_t box_of(const entity_t *e) {
    box_t b;
    b.left = (int32_t)e->x - e->w / 2;
    b.top = (int32_t)e->y - e->h / 2;
    b.right = b.left + e->w - 1;
    b.bottom = b.top + e->h - 1;
    return b;
}

static entity_events_t resolve_x(entity_t *e) {
    entity_events_t ev = 0;
    box_t b = box_of(e);
    if (e->dx > 0) {
        uint8_t f = scan_column(b.right, b.top, b.bottom);
        if (f & TILE_F_SOLID) {
            e->x = floor_div(b.right, TILE_SIZE) * TILE_SIZE - e->w + e->w / 2;
            e->dx = 0;
            if (f & TILE_F_GRIP) e->can_jump = true;   // wall jumping
        }
        if (f & TILE_F_HAZARD) ev |= ENT_EVT_HAZARD;
    } else if (e->dx < 0) {
        uint8_t f = scan_column(b.left, b.top, b.bottom);
        if (f & TILE_F_SOLID) {
            e->x = (floor_div(b.left, TILE_SIZE) + 1) * TILE_SIZE + e->w / 2;
            e->dx = 0;
            if (f & TILE_F_GRIP) e->can_jump = true;
        }
        if (f & TILE_F_HAZARD) ev |= ENT_EVT_HAZARD;
    }
    return ev;
}

static entity_events_t resolve_y(entity_t *e) {
    entity_events_t ev = 0;
    box_t b = box_of(e);
    if (e->dy > 0) {
        uint8_t f = scan_row(b.bottom, b.left, b.right);
        if (f & TILE_F_SOLID) {
            e->y = floor_div(b.bottom, TILE_SIZE) * TILE_SIZE - e->h + e->h / 2;
            e->dy = 0;
            ev |= ENT_EVT_LANDED;
            if (f & TILE_F_GRIP) e->can_jump = true;
        }
        if (f & TILE_F_HAZARD) ev |= ENT_EVT_HAZARD;
    } else if (e->dy < 0) {
        uint8_t f = scan_row(b.top, b.left, b.right);
        if (f & TILE_F_SOLID) {
            e->y = (floor_div(b.top, TILE_SIZE) + 1) * TILE_SIZE + e->h / 2;
            e->dy = 0;
        }
        if (f & TILE_F_HAZARD) ev |= ENT_EVT_HAZARD;
    }
    return ev;
}

// Which portals does the body overlap right now?
static entity_events_t portal_overlap(const entity_t *e) {
    entity_events_t ev = 0;
    box_t b = box_of(e);
    for (int32_t y = b.top;; y += TILE_SIZE) {
        if (y > b.bottom) y = b.bottom;
        for (int32_t x = b.left;; x += TILE_SIZE) {
            if (x > b.right) x = b.right;
            const tile_info_t *t = tile_info(map_tile_at(x, y));
            if (t->flags & TILE_F_PORTAL) ev |= ENT_EVT_PORTAL(t->portal);
            if (x == b.right) break;
        }
        if (y == b.bottom) break;
    }
    return ev;
}

void entity_init(entity_t *e, uint8_t hit_w, uint8_t hit_h, uint8_t draw_w, uint8_t draw_h,
                 int8_t health, uint16_t iframes_cap) {
    e->x = e->y = 0;
    e->dx = e->dy = 0;
    e->w = hit_w;
    e->h = hit_h;
    e->draw_w = draw_w;
    e->draw_h = draw_h;
    e->max_speed = MAX_CHAR_SPEED;
    e->health = health;
    e->iframes_cap = iframes_cap;
    e->iframes_counter = 0;
    e->can_jump = false;
    e->drawn = false;
}

void entity_place(entity_t *e, float x, float y) {
    e->x = x;
    e->y = y;
    e->dx = e->dy = 0;
    e->drawn = false;
}

void entity_integrate(entity_t *e, float ddx, float ddy) {
    e->dx += ddx;
    e->dy += ddy;

    if (ddx == 0.0f) {   // friction only when nothing is pushing sideways
        e->dx *= FRICTION;
        if (fabsf(e->dx) < 0.05f) e->dx = 0.0f;
    }

    if (e->dx > e->max_speed) e->dx = e->max_speed;
    else if (e->dx < -e->max_speed) e->dx = -e->max_speed;
    if (e->dy > e->max_speed) e->dy = e->max_speed;
    else if (e->dy < -e->max_speed) e->dy = -e->max_speed;
}

entity_events_t entity_move(entity_t *e) {
    entity_events_t ev = 0;

    // Split into sub-steps so nothing moves more than ENTITY_MAX_STEP at once.
    float biggest = fmaxf(fabsf(e->dx), fabsf(e->dy));
    int steps = (int)ceilf(biggest / ENTITY_MAX_STEP);
    if (steps < 1) steps = 1;

    for (int i = 0; i < steps; i++) {
        e->x += e->dx / steps;
        if (e->dx) ev |= resolve_x(e);
        e->y += e->dy / steps;
        if (e->dy) ev |= resolve_y(e);
    }

    ev |= portal_overlap(e);
    return ev;
}

void entity_tick_timers(entity_t *e) {
    if (e->iframes_counter) e->iframes_counter--;
}

bool entity_damage(entity_t *e) {
    if (e->iframes_counter) return false;
    e->health--;
    e->iframes_counter = e->iframes_cap;
    return true;
}

void entity_draw(entity_t *e, color_t color) {
    int32_t cx = (int32_t)e->x, cy = (int32_t)e->y;
    better_rect_fill(cx, cy, e->draw_w, e->draw_h, color);
    e->drawn_x = (int16_t)cx;
    e->drawn_y = (int16_t)cy;
    e->drawn = true;
}

void entity_erase(entity_t *e) {
    if (!e->drawn) return;
    map_draw_region(e->drawn_x - e->draw_w / 2, e->drawn_y - e->draw_h / 2, e->draw_w, e->draw_h);
    e->drawn = false;
}

void entity_forget_draw(entity_t *e) {
    e->drawn = false;
}
