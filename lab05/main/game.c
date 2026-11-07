#include "config.h"
#include "board.h"
#include "graphics.h"
#include "nav.h"
#include "com.h"
#include "game.h"

#include "hw.h"
#include "lcd.h"
#include "joy.h"
#include "pin.h"

#include <stdbool.h>
#include <stdint.h>

// grid defines
#define GRID_C CONFIG_BOARD_C
#define GRID_R CONFIG_BOARD_R

// static ints all functions need
static game_states curr_state;
static mark_t player;
static int8_t curr_location[2] = {0, 0};

// set initial state
void game_init(void) {
    curr_state = new_game;
}

// main game loop
void game_tick(void) {
    // actions
    switch(curr_state) {
        case new_game: {
            // clear the full screen
            lcd_fillScreen(CONFIG_BACK_CLR);
            board_clear();
            player = 1;
            nav_set_loc(GRID_R/2, GRID_C/2);

            // flush any bytes still in the com buffer
            uint8_t stray;
            while (com_read(&stray, 1) > 0) {}
            break;
        }
        // draw the grid on the board action
        case draw_grid: {
            graphics_drawGrid(CONFIG_GRID_CLR);
            break;
        }
        //write message for whose turn in the beginning
        case begin_msg: {
            graphics_drawMessage("Player X begin", CONFIG_MESS_CLR, BLACK);
            break;
        }
        // see where the cursor is action
        case wait_mark: {
            nav_get_loc(&curr_location[0], &curr_location[1]);
            break;
        }
        // draw the actual mark
        case mark: {
            // only the mark is drawn this tick
            player == 1 ? graphics_drawX(curr_location[0], curr_location[1], CONFIG_MARK_CLR)
                        : graphics_drawO(curr_location[0], curr_location[1], CONFIG_MARK_CLR);
            break;
        }
        // pass the turn to the next player, switch x for 0
        case next_turn: {
            player = (player == 1) ? 2 : 1;
            player == 1 ? graphics_drawMessage("Player X's turn", CONFIG_MESS_CLR, BLACK)
                        : graphics_drawMessage("Player O's turn", CONFIG_MESS_CLR, BLACK);
            break;
        }
        // somebody won!!!
        case win_msg: {
            player == 1 ? graphics_drawMessage("Player X won!!!!!", CONFIG_MESS_CLR, BLACK)
                        : graphics_drawMessage("Player O won!!!!!", CONFIG_MESS_CLR, BLACK);
            break;
        }
        // nobody won :(
        case draw_msg: {
            graphics_drawMessage("It is a draw!!!!!", CONFIG_MESS_CLR, BLACK);
            break;
        }
        //state after game has ended
        case wait_restart: {
            break;
        }
        default: {
            break;
        }
    }

    // transition state machine
    switch(curr_state) {
        // new game transition state
        case new_game: {
            curr_state = draw_grid;
            break;
        }
        // move to the message
        case draw_grid: {
            curr_state = begin_msg;
            break;
        }
        //wait for a mark to appear
        case begin_msg: {
            curr_state = wait_mark;
            break;
        }
        // send communication to the other gameboy thing
        case wait_mark: {
            bool do_mark = false;

            // button A sends the location to the other machine
            if (!pin_get_level(HW_BTN_A)) {
                uint8_t tx = (uint8_t)(((curr_location[0] & 0x0F) << 4) |
                                       (curr_location[1] & 0x0F));
                com_write(&tx, 1);
                if (!board_get(curr_location[0], curr_location[1])) {
                    do_mark = true;
                }
            }

            // We got something in return!!!
            if (!do_mark) {
                uint8_t rx;
                if (com_read(&rx, 1) > 0) {
                    int8_t row = rx >> 4;
                    int8_t col = rx & 0x0F;
                    if (row < GRID_R && col < GRID_C && !board_get(row, col)) {
                        curr_location[0] = row;
                        curr_location[1] = col;
                        do_mark = true;
                    }
                }
            }

            // place the mark, the draw functions in action state machine will handle this
            if (do_mark) {
                board_set(curr_location[0], curr_location[1], player);
                curr_state = mark;
            }
            break;
        }
        case mark: {
            // check the win before the player is swapped
            if (board_winner(player)) {
                curr_state = win_msg;
            } else if (board_mark_count() == 9) {
                curr_state = draw_msg;
            } else {
                curr_state = next_turn;
            }
            break;
        }
        // transition to wait_mark
        case next_turn: {
            curr_state = wait_mark;
            break;
        }
        // both of these cases want to wait_restart
        case win_msg:
        //same as win_msg
        case draw_msg: {
            curr_state = wait_restart;
            break;
        }
        //wait until we hit start, and then start a new game
        case wait_restart: {
            if (!pin_get_level(HW_BTN_START)) {
                curr_state = new_game;
            }
            break;
        }
        default: {
            break;
        }
    }
}

