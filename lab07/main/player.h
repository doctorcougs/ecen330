#ifndef PLAYER_H
#define PLAYER_H
#include <stdint.h>

#include "draw.h"
#include "config.h"

typedef struct {

    float x;
    float y;
    float dx;
    float dy;
    float ddx;
    float ddy;

    int8_t health;
    uint16_t iframes_cap;
    uint16_t iframes_counter;

    uint8_t level_counter;
    bool shift_level;
    
    int32_t currentState;

    uint8_t can_jump;

}player_t;

void player_init(player_t *player);

void player_tick(player_t *player);

void player_draw(player_t *player);

void player_get_location(player_t *player);
void player_set_location(player_t *player, int32_t x, int32_t y);
void player_take_damage(player_t *player);
#endif // PLAYER_H