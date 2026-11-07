#include "sprite.h"

void sprite_draw(int32_t x, int32_t y, const sprite_t *s, color_t color) {
    lcd_drawBitmap(x, y, s->data, s->w, s->h, color);
}

void sprite_draw_centered(int32_t cx, int32_t cy, const sprite_t *s, color_t color) {
    sprite_draw(cx - s->w / 2, cy - s->h / 2, s, color);
}
