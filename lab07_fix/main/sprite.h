#ifndef SPRITE_H
#define SPRITE_H
#include <stdint.h>

#include "draw.h"

// A 1-bit bitmap plus its size, as exported by the sprite tool.
typedef struct {
    const uint8_t *data;
    uint8_t w, h;
} sprite_t;

void sprite_draw(int32_t x, int32_t y, const sprite_t *s, color_t color);           // x, y = top-left
void sprite_draw_centered(int32_t cx, int32_t cy, const sprite_t *s, color_t color); // x, y = center

#endif // SPRITE_H
