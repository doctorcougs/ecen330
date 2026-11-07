#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdint.h>

// One snapshot of the controls, sampled once per tick by input_sample() and
// then handed to whoever needs it (player, menus, boss fights...).
typedef struct {
    int8_t move_x;          // -1 left, 0 none, +1 right
    bool jump_held;         // A is down
    bool jump_pressed;      // A went down this tick (edge)
    bool start_held;
    bool start_pressed;     // START went down this tick (edge)

    bool prev_jump, prev_start;   // for edge detection, don't touch
} input_t;

void input_reset(input_t *in);   // zero it and swallow any button already held
void input_sample(input_t *in);

#endif // INPUT_H
