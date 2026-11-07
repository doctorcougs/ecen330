#include <stdio.h>
#include <stdlib.h>

#include "esp_random.h"
#include "hw.h"
#include "pin.h"

#include "config.h"
#include "game.h"
#include "input.h"
#include "ui.h"
#include "world.h"

static game_state_t game_state;
static world_t world;
static input_t input;

#define OVERLAY_DIGITS 3

// Health top-right, level number top-left. Cleared through the map so a tile
// or the player passing underneath isn't wiped out.
static void overlay_draw(void) {
    char text[16];
    int32_t w = OVERLAY_DIGITS * FONT_W;

    map_draw_region(LCD_W - w, 0, w, FONT_H);
    snprintf(text, sizeof(text), "%3d", (int)world.player.body.health);
    lcd_drawString(LCD_W - w, 0, text, GRAY);

    map_draw_region(0, 0, w, FONT_H);
    snprintf(text, sizeof(text), "%3d", (int)world.level_id);
    lcd_drawString(0, 0, text, GRAY);
}

void game_init(void) {
    game_state = GAME_IDLE;
}

void game_tick(main_state_t *main_state) {
    switch (game_state) {
        case GAME_IDLE: {
            lcd_fillScreen(CONFIG_COLOR_BACKGROUND);
            world_init(&world, (level_id_t)(esp_random() % MAP_COUNT));
            input_reset(&input);
            game_state = GAME_LOAD_MAP;
            break;
        }
        // entered again after every level change
        case GAME_LOAD_MAP: {
            lcd_fillScreen(CONFIG_COLOR_BACKGROUND);
            map_draw();
            entity_forget_draw(&world.player.body);
            game_state = GAME_WAIT;
            break;
        }
        // game running on same screen: update, then erase/redraw only what moved
        case GAME_WAIT: {
            input_sample(&input);
            entity_events_t ev = player_tick(&world.player, &input);

            player_erase(&world.player);
            if (player_is_dead(&world.player)) {
                game_state = GAME_OVER;
                break;
            }
            for (int k = 0; k < PORTAL_COUNT; k++) {   // lowest kind wins if several overlap
                if ((ev & ENT_EVT_PORTAL(k)) && world_take_portal(&world, (portal_kind_t)k)) {
                    game_state = GAME_LOAD_MAP;
                    break;
                }
            }
            if (game_state == GAME_WAIT) {
                overlay_draw();
                player_draw(&world.player);
            }
            break;
        }
        // Placeholders so adding pause / boss intro / victory later doesn't
        // mean re-plumbing the state machine; nothing enters them yet.
        case GAME_PAUSED: {
            input_sample(&input);
            if (input.start_pressed) game_state = GAME_WAIT;
            break;
        }
        case GAME_BOSS_INTRO: {
            game_state = GAME_WAIT;
            break;
        }
        case GAME_VICTORY: {
            game_state = GAME_OVER;
            break;
        }
        // back to the start screen; GAME_IDLE re-inits everything next time
        case GAME_OVER: {
            *main_state = MAIN_IDLE;
            game_state = GAME_IDLE;
            break;
        }
        default: {
            break;
        }
    }
}
