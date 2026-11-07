#include "player.h"

#include "tuning.h"

void player_init(player_t *player) {
    entity_init(&player->body, PLAYER_HITBOX_W, PLAYER_HITBOX_H, PLAYER_DRAW_W, PLAYER_DRAW_H,
                PLAYER_STARTING_HEALTH, PLAYER_STARTING_IFRAMES_CAP);
    player->state = PLAYER_IDLE;
}

void player_spawn(player_t *player, float x, float y) {
    entity_place(&player->body, x, y);
}

entity_events_t player_tick(player_t *player, const input_t *input) {
    entity_events_t ev = 0;
    entity_t *e = &player->body;

    switch (player->state) {
        case PLAYER_IDLE: {
            player->state = PLAYER_MOVEMENT;
            break;
        }
        case PLAYER_MOVEMENT: {
            entity_tick_timers(e);

            if (input->start_pressed) {   // suicide button
                e->health = 0;
            }

            float ddx = input->move_x * CHARACTER_ACCEL_PER_TICK;
            float ddy = GRAVITY_ACCEL_PER_TICK;
            if (input->jump_held && e->can_jump) {
                ddy -= CHARACTER_JUMP_PER_TICK;
                e->can_jump = false;
            }

            entity_integrate(e, ddx, ddy);
            ev = entity_move(e);

            if (ev & ENT_EVT_HAZARD) {
                entity_damage(e);
            }
            if (!entity_alive(e)) {
                player->state = PLAYER_DEAD;
            }
            break;
        }
        case PLAYER_DEAD: {
            break;
        }
    }
    return ev;
}

void player_draw(player_t *player) {
    entity_draw(&player->body, RED);
}

void player_erase(player_t *player) {
    entity_erase(&player->body);
}
