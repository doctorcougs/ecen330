#ifndef GAME_H_
#define GAME_H_

// enum for different states
typedef enum {
    new_game,      // clear screen, reset board
    draw_grid,     // draw the grid
    begin_msg,     // "Player X begin"
    wait_mark,
    mark,          // draw the X/O
    next_turn,     // swap player, draw turn message
    win_msg,       // draw winner message
    draw_msg,      // draw "draw" message
    wait_restart
} game_states;

// Initialize the game logic.
void game_init(void);

// Update the game logic.
void game_tick(void);

#endif // GAME_H_
