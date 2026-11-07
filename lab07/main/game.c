#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "pin.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "esp_random.h"

static game_state_t game_state;
static map_state_t map_state;
player_t player;

static void intToString(coord_t x, char *str) {
	sprintf(str, "%3ld", x);
}

static void overlay_draw(void) {
    char health[24];
    intToString(player.health, health);
    lcd_drawString(LCD_W - 3 * FONT_W, 0, health, GRAY);
    char level[24];
    intToString(player.level_counter, level);
    lcd_drawString(0, 0, level, GRAY);
}

void game_init(void) {
    game_state = GAME_IDLE;
}

void game_tick(main_state_t *main_state) {
    switch (game_state) {
        case GAME_IDLE: {
            lcd_fillScreen(BLACK);
            player_init(&player);
            player.level_counter = (uint8_t)(esp_random() % MAP_COUNT);
            game_state = GAME_LOAD_MAP;
            break;
        }
        // when passing to a new screen game load map will run with that screen
        case GAME_LOAD_MAP: {
            map_state = (map_state_t)player.level_counter;
            map_load(map_state);
            map_draw();
            game_state = GAME_WAIT;
            break;
        }
        // game running on same screen
        case GAME_WAIT: {
            lcd_fillScreen(BLACK);
            map_draw();
            overlay_draw();
            player_tick(&player);
            //if we dead go back to main idle(starting screen) and reset gamestate to idle so we can reinit player
            if (!player.health) {
                *main_state = MAIN_IDLE;
                game_state = GAME_IDLE;
            } else if (player.shift_level) {
                game_state = GAME_LOAD_MAP;
                player.shift_level = false;
            }
            break;
        }
        default: {
            break;
        }
    }
}