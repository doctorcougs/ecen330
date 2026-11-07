#ifndef STARTING_SCREEN_H
#define STARTING_SCREEN_H

#include <stdbool.h>
#include <stdint.h>

#include "draw.h"
#include "config.h"

typedef enum {
    SCREEN_IDLE,
    SCREEN_WAITING,
    SCREEN_SELECTING,
    SCREEN_CHOOSE,
} starting_screen_state_t;

typedef enum {
    SCREEN_START,
    SCREEN_OPTIONS,
    SCREEN_QUIT,
    SELECTION_COUNT,
} destinations_t;

void starting_screen_init(void);

void starting_screen_tick(main_state_t *main_state);


#endif // STARTING_SCREEN_H