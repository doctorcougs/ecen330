#ifndef GAME_H
#define GAME_H
#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "main_state.h"

typedef enum {
    GAME_IDLE,        // set up a fresh world
    GAME_LOAD_MAP,    // (re)draw the current level
    GAME_WAIT,        // normal play
    GAME_PAUSED,      // not entered yet
    GAME_BOSS_INTRO,  // not entered yet
    GAME_VICTORY,     // not entered yet
    GAME_OVER,        // player died, hand control back to the menu
} game_state_t;

void game_init(void);

void game_tick(main_state_t *main_state);

#endif // GAME_H
