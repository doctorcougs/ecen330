#include "starting_screen.h"

#include "joy.h"
#include "pin.h"


static starting_screen_state_t state;
static destinations_t selection;
static int32_t joy_x,joy_y;

//init requirements:
// set initial state
// draw screen
void starting_screen_init(void) {
    state = SCREEN_IDLE;
    selection = SCREEN_START;
    joy_x = joy_y = 0;
}

//handles the wrap around for select options
//bool inc 1 decrements, 0 increments
static void change_selection(bool inc) {
    if (inc) {
        selection = (starting_screen_state_t)((selection + SELECTION_COUNT - 1) % SELECTION_COUNT);
    } else {
        selection = (starting_screen_state_t)((selection + 1) % SELECTION_COUNT);
    }
}

static void draw_screen(void) {
    lcd_fillScreen(CONFIG_COLOR_BACKGROUND);
    better_text(HALF_SCREEN_W, HALF_SCREEN_H / 2, "GAME", WHITE);
    better_rect(HALF_SCREEN_W, HALF_SCREEN_H, MEDIUM_BOX_W, MEDIUM_BOX_H, WHITE);
    better_text(HALF_SCREEN_W, HALF_SCREEN_H, "start", WHITE);
    better_rect(HALF_SCREEN_W, HALF_SCREEN_H + MEDIUM_BOX_H + MEDIUM_BOX_SPACING, MEDIUM_BOX_W, MEDIUM_BOX_H, WHITE);
    better_text(HALF_SCREEN_W, HALF_SCREEN_H + MEDIUM_BOX_H + MEDIUM_BOX_SPACING, "options", WHITE);
    better_rect(HALF_SCREEN_W, HALF_SCREEN_H + 2 * (MEDIUM_BOX_H + MEDIUM_BOX_SPACING), MEDIUM_BOX_W, MEDIUM_BOX_H, WHITE);
    better_text(HALF_SCREEN_W, HALF_SCREEN_H + 2 * (MEDIUM_BOX_H + MEDIUM_BOX_SPACING), "quit", WHITE);
}

void starting_screen_tick(main_state_t *main_state) {
    switch(state) {
        case SCREEN_IDLE: {
            draw_screen();
            better_rect(HALF_SCREEN_W, HALF_SCREEN_H, MEDIUM_BOX_W, MEDIUM_BOX_H, RED);
            state = SCREEN_WAITING;
            break;
        }
        // if screen is waiting I want it to:
        // reads inputs dictates what states need to go
        case SCREEN_WAITING: {
            // TODO: make a tolerance value for joy_get_dispalcement
            joy_get_displacement(&joy_x, &joy_y);
            // Convert from joystick displacement to screen coordinates
            if (joy_y > JOYSTICK_SELECT_SENSE) {
                change_selection(0);
                state = SCREEN_SELECTING;
            } else if (joy_y < -JOYSTICK_SELECT_SENSE) {
                change_selection(1);
                state = SCREEN_SELECTING;
            } else if (!pin_get_level(HW_BTN_A)) {
                state = SCREEN_CHOOSE;
            }
            break;
        }
        //draws box around selected item
        case SCREEN_SELECTING: {
            for (uint8_t i = 0; i < SELECTION_COUNT; i++) {
                better_rect(HALF_SCREEN_W, HALF_SCREEN_H + i * (MEDIUM_BOX_H + MEDIUM_BOX_SPACING), MEDIUM_BOX_W, MEDIUM_BOX_H, WHITE);
            }
            better_rect(HALF_SCREEN_W, HALF_SCREEN_H + selection * (MEDIUM_BOX_H + MEDIUM_BOX_SPACING), MEDIUM_BOX_W, MEDIUM_BOX_H, RED);
            joy_get_displacement(&joy_x, &joy_y);
            if (joy_y < JOYSTICK_SELECT_SENSE && joy_y > -JOYSTICK_SELECT_SENSE) {
                state = SCREEN_WAITING;
            }
            break;
        }
        case SCREEN_CHOOSE: {
            switch (selection) {
                case SCREEN_START: 
                    *main_state = MAIN_START;
                    state = SCREEN_IDLE;
                    break;
                case SCREEN_OPTIONS:
                    *main_state = MAIN_OPTIONS;
                    state = SCREEN_IDLE;
                    break;
                case SCREEN_QUIT:
                    *main_state = MAIN_QUIT;
                    break;
                default:
                    state = SCREEN_WAITING;
                    break;
            }
        }
        default: {
            break; 
        }
    }

}