#ifndef GAME_H
#define GAME_H
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "config.h"

typedef enum {
    GAME_IDLE,
    GAME_LOAD_MAP,
    GAME_WAIT,
} game_state_t;

void game_init(void);

void game_tick(main_state_t *main_state);

void game_set_random(int seed);


#endif // GAME_H