#include "draw.h"
#include "config.h"
#include "string.h"

void better_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color) {
    lcd_drawRect(x - (w / 2), y - (h / 2), w, h, color);
}

void better_rect_fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, color_t color) {
    lcd_fillRect(x - (w / 2), y - (h / 2), w, h, color);
}

void better_text(uint32_t x, uint32_t y, const char *text, color_t color) {
    uint32_t text_w = strlen(text) * FONT_W;
    lcd_drawString(x - (text_w / 2), y - (FONT_H / 2), text, color);
}