#include "player.h"
#include "math.h"
#include "config.h"
#include "hw.h"
#include "pin.h"
#include "joy.h"
#include "map.h"

#include <stdio.h>

typedef enum {
    PLAYER_IDLE,
    PLAYER_MOVEMENT,
} player_state_t;

static int32_t joy_x, joy_y;

static inline int32_t floor_div(int32_t a, int32_t b) {
    int32_t q = a / b;
    if ((a % b != 0) && ((a < 0) != (b < 0))) q--;
    return q;
}

static void player_collide_x(player_t *player) {
    int32_t x = (int32_t)player->x;
    int32_t y = (int32_t)player->y;
    int32_t left   = x - HALF_CHARACTER_SIZE;
    int32_t right  = x + HALF_CHARACTER_SIZE - 1;
    int32_t top    = y - HALF_CHARACTER_SIZE;
    int32_t bottom = y + HALF_CHARACTER_SIZE - 1;
    // right collision, include top and bottom for corner cases and wall jumping
    if (player->dx > 0) {
        if ((map_solid_at(right, top) == '#' || map_solid_at(right, bottom) == '#')) {
            player->x = (right / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dx = 0;
            player->can_jump = 1;
        } else if ((map_solid_at(right, top) == '%' || map_solid_at(right, bottom) == '%')) {
            player->x = (right / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dx = 0;
        }
        if ((map_solid_at(right, top) == 'X' || map_solid_at(right, bottom) == 'X')) {
            player->x = (right / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dx = 0;
            player_take_damage(player);
        } 
        if ((map_solid_at(right, top) == '>' || map_solid_at(right, bottom) == '>')) {
            player->level_counter++;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = 0;
            }
            player->shift_level = true;
        } else if ((map_solid_at(right, top) == '<' || map_solid_at(right, bottom) == '<')) {
            player->level_counter--;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = MAP_COUNT -1;
            }
            player->shift_level = true;
        } else if ((map_solid_at(right, top) == '*' || map_solid_at(right, bottom) == '*')) {
            player->level_counter *= 2;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter %= MAP_COUNT;
            }
            player->shift_level = true;
        } else if ((map_solid_at(right, top) == '/' || map_solid_at(right, bottom) == '/')) {
            player->level_counter /= 2;
            player->shift_level = true;
        }
    }// left collision, smae include of top and bottom 
    else if (player->dx < 0) {
        if ((map_solid_at(left, top) == '#' || map_solid_at(left, bottom) == '#')) {
            player->x = (floor_div(left, TILE_SIZE) + 1) * TILE_SIZE + HALF_CHARACTER_SIZE;
            player->dx = 0;
            player->can_jump = 1;
        } else if ((map_solid_at(left, top) == '%' || map_solid_at(left, bottom) == '%')) {
            player->x = (floor_div(left, TILE_SIZE) + 1) * TILE_SIZE + HALF_CHARACTER_SIZE;
            player->dx = 0;
        }
        if ((map_solid_at(left, top) == 'X' || map_solid_at(left, bottom) == 'X')) {
            player_take_damage(player);
            player->x = (floor_div(left, TILE_SIZE) + 1) * TILE_SIZE + HALF_CHARACTER_SIZE;
            player->dx = 0;
        } 
        if ((map_solid_at(left, top) == '>' || map_solid_at(left, bottom) == '>')) {
            player->level_counter++;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = 0;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '<' || map_solid_at(left, bottom) == '<')) {
            player->level_counter--;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = MAP_COUNT - 1;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '*' || map_solid_at(left, bottom) == '*')) {
            player->level_counter *= 2;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter %= MAP_COUNT;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '/' || map_solid_at(left, bottom) == '/')) {
            player->level_counter /= 2;
            player->shift_level = true;
        }
    }
}

static void player_collide_y(player_t *player) {
    int32_t x = (int32_t)player->x;
    int32_t y = (int32_t)player->y;
    int32_t left   = x - HALF_CHARACTER_SIZE;
    int32_t right  = x + HALF_CHARACTER_SIZE - 1;
    int32_t top    = y - HALF_CHARACTER_SIZE;
    int32_t bottom = y + HALF_CHARACTER_SIZE - 1;

    // bottom collision, also can reset jump, check left and right for coner cases
    if (player->dy > 0) {
        if ((map_solid_at(left, bottom) == '#' || map_solid_at(right, bottom) == '#')) {
            player->y = (bottom / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dy = 0;
            player->can_jump = 1;
        } else if ((map_solid_at(left, bottom) == '%' || map_solid_at(right, bottom) == '%')) {
            player->y = (bottom / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dy = 0;
        }
        if ((map_solid_at(left, bottom) == 'X' || map_solid_at(right, bottom) == 'X')) {
            player_take_damage(player);
            player->y = (bottom / TILE_SIZE) * TILE_SIZE - HALF_CHARACTER_SIZE;
            player->dy = 0;
        } 
        if ((map_solid_at(left, bottom) == '>' || map_solid_at(right, bottom) == '>')) {
            player->level_counter++;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = 0;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, bottom) == '<' || map_solid_at(right, bottom) == '<')) {
            player->level_counter--;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = MAP_COUNT -1;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, bottom) == '*' || map_solid_at(right, bottom) == '*')) {
            player->level_counter *= 2;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter %= MAP_COUNT;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, bottom) == '/' || map_solid_at(right, bottom) == '/')) {
            player->level_counter /= 2;
            player->shift_level = true;
        }
    }
    // top collision, check corners
    else if (player->dy < 0) {
        if ((map_solid_at(left, top) == '#' || map_solid_at(right, top) == '#') || (map_solid_at(left, top) == '%' || map_solid_at(right, top) == '%')) {
            player->y = (floor_div(top, TILE_SIZE) + 1) * TILE_SIZE + HALF_CHARACTER_SIZE;
            player->dy = 0;
        }
        if ((map_solid_at(left, top) == 'X' || map_solid_at(right, top) == 'X')) {
            player_take_damage(player);
            player->y = (floor_div(top, TILE_SIZE) + 1) * TILE_SIZE + HALF_CHARACTER_SIZE;
            player->dy = 0;
        } 
        if ((map_solid_at(left, top) == '>' || map_solid_at(right, top) == '>')) {
            player->level_counter++;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = 0;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '<' || map_solid_at(right, top) == '<')) {
            player->level_counter--;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter = MAP_COUNT -1;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '*' || map_solid_at(right, top) == '*')) {
            player->level_counter *= 2;
            if (player->level_counter >= MAP_COUNT) {
                player->level_counter %= MAP_COUNT;
            }
            player->shift_level = true;
        } else if ((map_solid_at(left, top) == '/' || map_solid_at(right, top) == '/')) {
            player->level_counter /= 2;
            player->shift_level = true;
        }
    }
}

void player_init(player_t *player) {
    player->x = HALF_SCREEN_W;
    player->y = HALF_SCREEN_H;
    player->dx = 0;
    player->dy = 0;
    player->ddx = 0;
    player->ddy = 0;
    player->can_jump = 0;

    player->iframes_cap = PLAYER_STARTING_IFRAMES_CAP;
    player->iframes_counter = player->iframes_cap;

    player->health = PLAYER_STARTING_HEALTH;

    player->currentState = PLAYER_IDLE;
    joy_x = 0;
    joy_y = 0;
}

void player_tick(player_t *player) {
    switch (player->currentState) {
        case PLAYER_IDLE: {
            player->currentState = PLAYER_MOVEMENT;
            break;
        }
        case PLAYER_MOVEMENT: {
            //check for suicide
            if (!pin_get_level(HW_BTN_START)) {
                player->health = 0;
            }
            //check for movement, apply forces
            // printf("player_wait");
            joy_get_displacement(&joy_x, &joy_y);
            player->ddx = 0;
            if (joy_x > JOYSTICK_MOVE_SENSE) {
                player->ddx += CHARACTER_ACCEL_PER_TICK;
            }
            if (joy_x < -JOYSTICK_MOVE_SENSE) {
                player->ddx -= CHARACTER_ACCEL_PER_TICK;
            }
            //gravity
            player->ddy = GRAVITY_ACCEL_PER_TICK;
            //jumping
            if (!pin_get_level(HW_BTN_A) && player->can_jump) {
                player->ddy -= CHARACTER_JUMP_PER_TICK;
                player->can_jump =0;
            }

            player->dx += player->ddx;
            player->dy += player->ddy;

            //x friction
            if (player->ddx == 0.0f) {
                player->dx *= FRICTION;
                if (fabsf(player->dx) < 0.05f) {
                    player->dx = 0.0f;
                }
            }
            
            //bounds checking speed
            // x
            if (player->dx > MAX_CHAR_SPEED) {
                player->dx = MAX_CHAR_SPEED;
            } else if (player->dx < -MAX_CHAR_SPEED) {
                player->dx = -MAX_CHAR_SPEED;
            }
            // y
            if (player->dy > MAX_CHAR_SPEED) {
                player->dy = MAX_CHAR_SPEED;
            } else if (player->dy < -MAX_CHAR_SPEED) {
                player->dy = -MAX_CHAR_SPEED;
            }

            //collision checking
            // x
            player->x += player->dx;
            if (player->dx) {
                player_collide_x(player);
            }
            // y
            player->y += player->dy;
            if (player->dy) {
                player_collide_y(player);
            }

            player_draw(player);
            break;
        }
    }
}

void player_draw(player_t *player) {
    better_rect_fill((int32_t)player->x, (int32_t)player->y, CHARACTER_SIZE, CHARACTER_SIZE, RED);
}

void player_get_location(player_t *player) {

}

void player_set_location(player_t *player, int32_t x, int32_t y) {

}

void player_take_damage(player_t *player) {
    if (!player->iframes_counter) {
        player->health--;
        player->iframes_counter = player->iframes_cap;
    }
    player->iframes_counter--;
}
