#ifndef MAIN_STATE_H
#define MAIN_STATE_H

// Top-level state machine owned by main.c; the menu and the game both use it
// to ask main to switch screens.
typedef enum {
    MAIN_IDLE,
    MAIN_START,
    MAIN_OPTIONS,
    MAIN_QUIT,
} main_state_t;

#endif // MAIN_STATE_H
