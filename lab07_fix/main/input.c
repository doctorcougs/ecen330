#include "input.h"

#include "hw.h"
#include "joy.h"
#include "pin.h"
#include "tuning.h"

void input_reset(input_t *in) {
    in->move_x = 0;
    in->jump_held = in->jump_pressed = false;
    in->start_held = in->start_pressed = false;
    // buttons are active-low; treat whatever is down right now as "already held"
    in->prev_jump = !pin_get_level(HW_BTN_A);
    in->prev_start = !pin_get_level(HW_BTN_START);
}

void input_sample(input_t *in) {
    int32_t joy_x = 0, joy_y = 0;
    joy_get_displacement(&joy_x, &joy_y);
    in->move_x = 0;
    if (joy_x > JOYSTICK_MOVE_SENSE) in->move_x += 1;
    if (joy_x < -JOYSTICK_MOVE_SENSE) in->move_x -= 1;

    in->jump_held = !pin_get_level(HW_BTN_A);
    in->jump_pressed = in->jump_held && !in->prev_jump;
    in->prev_jump = in->jump_held;

    in->start_held = !pin_get_level(HW_BTN_START);
    in->start_pressed = in->start_held && !in->prev_start;
    in->prev_start = in->start_held;
}
